#include "engine.hpp"

Engine::Engine() {
	_initSDL();
	_initGL();
	_initShaders();
	_initMeshes();
}

Engine::~Engine() {
	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

int Engine::run() {
	_quit = false;
	while (!_quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				_quit = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					_quit = true;
				break; 
			}
		}

		
		glClearColor(0.0, 0.13, 0.13*2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		_mesh->render(glm::mat4(1.0));
		
		SDL_GL_SwapWindow(_window);
	}
	return 0;
}



void Engine::_initSDL() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		throw "SDL could not be inited";
	
	_window = SDL_CreateWindow("Lab2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (!_window)
		throw "Failed to create window";
}

void Engine::_initGL() {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		
	_context = SDL_GL_CreateContext(_window);
	glewExperimental = GL_TRUE;
	if (glewInit())
		throw "Failed to init glew";

	SDL_GL_SetSwapInterval(1);
	glEnable(GL_DEPTH_TEST);
}

void Engine::_initShaders() {
	{
		_baseProgram = std::make_shared<ShaderProgram>();
		_baseProgram
			->attach(std::make_shared<ShaderUnit>("assets/shaders/base.vert", ShaderType::vertex))
			 .attach(std::make_shared<ShaderUnit>("assets/shaders/base.frag", ShaderType::fragment))
	 		 .finalize();
		//_baseProgram->addUniform("mvp");
	}
}

void Engine::_initMeshes() {
	{
		std::vector<Vertex> vertices {
			Vertex {
				glm::vec4{-0.5f, 0.5f, 0.0f, 0.0f},
				glm::vec4{0.0f, 0.0f, 0.0f, 0.0f},
				glm::vec4{1.0f, 0.0f, 0.0f, 0.0f}
			}, // Top-left
			Vertex {
				glm::vec4{0.5f, 0.5f, 0.0f, 0.0f},
				glm::vec4{0.0f, 0.0f, 0.0f, 0.0f},
				glm::vec4{0.0f, 1.0f, 0.0f, 0.0f}
			}, // Top-right
			Vertex {
				glm::vec4{0.5f, -0.5f, 0.0f, 0.0f},
				glm::vec4{0.0f, 0.0f, 0.0f, 0.0f},
				glm::vec4{0.0f, 0.0f, 1.0f, 0.0f}
			}, // Bottom-right
			Vertex {
				glm::vec4{-0.5f, -0.5f, 0.0f, 0.0f},
				glm::vec4{0.0f, 0.0f, 0.0f, 0.0f},
				glm::vec4{1.0f, 1.0f, 1.0f, 0.0f}
			} // Bottom-left
		};

		std::vector<GLuint> indicies {
			0, 1, 2,
			2, 3, 0
		};

		_mesh = std::make_shared<Mesh>(_baseProgram, vertices, indicies);
	}
}
