#include "engine.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

Engine::Engine() {
	_initSDL();
	_initGL();
	_initShaders();
	_initMeshes();
}

Engine::~Engine() {
	IMG_Quit();
	
	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

int Engine::run() {
	_quit = false;
	int fps;
	uint32_t lastTime = SDL_GetTicks();
	
	_updateMatrices(0, false);
	bool updateCamera = false;
	while (!_quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				_quit = true;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					_quit = true;
					break;
				case SDLK_LSHIFT:
				case SDLK_RSHIFT:
					_speed = 2.5f;
					break;
				default:
					break;
				}
				
				break;
				
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
				case SDLK_LSHIFT:
				case SDLK_RSHIFT:
					_speed = 5.0f;
					break;
				default:
					break;
				}
				
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_RIGHT) {
					updateCamera = true;
					SDL_ShowCursor(false);
					SDL_WarpMouseInWindow(_window, _width/2, _height/2);
				}
				break;
				
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_RIGHT) {
					updateCamera = false;
					SDL_ShowCursor(true);
				}
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					_width = event.window.data1;
					_height = event.window.data2;
				}
				break;
			default:
				break;
			}
		}

		uint32_t curTime = SDL_GetTicks();
		float delta = (curTime - lastTime)/1000.0f;
		lastTime = curTime;
		_updateMatrices(delta, updateCamera);

		//glClearColor(0.0, 0.13, 0.26, 1.0);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_mesh->getTranslation() *= glm::rotate(delta, glm::vec3(0, 3, 0));
		glm::mat4 mv = _view * _mesh->getTranslation();
		glm::mat4 p = _projection;
		
		_mesh->render(p * mv, mv);

		fps++;
		SDL_GL_SwapWindow(_window);
	}
	return 0;
}



void Engine::_initSDL() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		throw "SDL could not be inited";
	
	_window = SDL_CreateWindow("Lab2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width, _height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_2D);

	glViewport(0, 0, _width, _height);

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
		throw "Failed to load SDL_Image";

	
}

void Engine::_initShaders() {
	{
		_baseProgram = std::make_shared<ShaderProgram>();
		_baseProgram
			->attach(std::make_shared<ShaderUnit>("assets/shaders/base.vert", ShaderType::vertex))
			.attach(std::make_shared<ShaderUnit>("assets/shaders/base.frag", ShaderType::fragment))
			.attach(std::make_shared<ShaderUnit>("assets/shaders/base.geom", ShaderType::geometry))
	 		.finalize();
		_baseProgram->addUniform("mvp").addUniform("mv").addUniform("tex").addUniform("diffusePos");
	}
}

void Engine::_initMeshes() {
	{
		std::vector<Vertex> vertices {
			Vertex { // Top-left
				glm::vec3{-0.5f, 0.5f, 0.0f}, // pos
 				glm::vec3{0.0f, 0.0f, 0.0f},  // normal
				glm::vec3{0.0f, 0.0f, 0.0f},  // color
				glm::vec2{0.0f, 0.0f}         // UV
			},
			Vertex { // Top-right
				glm::vec3{0.5f, 0.5f, 0.0f},
				glm::vec3{0.0f, 0.0f, 0.0f},
				glm::vec3{1.0f, 0.0f, 0.0f},
				glm::vec2{1.0f, 0.0f}
			},
			Vertex { // Bottom-right
				glm::vec3{0.5f, -0.5f, 0.0f},
				glm::vec3{0.0f, 0.0f, 0.0f},
				glm::vec3{1.0f, 1.0f, 0.0f},
				glm::vec2{1.0f, 1.0f}
			},
			Vertex { // Bottom-left
				glm::vec3{-0.5f, -0.5f, 0.0f},
				glm::vec3{0.0f, 0.0f, 0.0f},
				glm::vec3{0.0f, 1.0f, 0.0f},
				glm::vec2{0.0f, 1.0f}
			}
		};

		std::vector<GLuint> indicies {
			0, 2, 1,
			2, 0, 3
		};

		_mesh = std::make_shared<Mesh>(_baseProgram, vertices, indicies, "assets/textures/bth.png");
	}
}

void Engine::_updateMatrices(float delta, bool updateCamera) {
	if (updateCamera) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		SDL_WarpMouseInWindow(_window, _width/2, _height/2);

		x = _width/2 - x;
		y = _height/2 - y;
		
		float mspeed = 0.005f;
		_yaw += mspeed * x;
		_pitch += mspeed * y;
	}
	glm::vec3 forward(cos(_pitch) * sin(_yaw), sin(_pitch), cos(_pitch) * cos(_yaw));
	glm::vec3 right(sin(_yaw - M_PI/2.0f), 0, cos(_yaw - M_PI/2.0f));
	glm::vec3 up = glm::cross(right, forward);

	const uint8_t * state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_W])
		_position += forward * delta * _speed;
	if (state[SDL_SCANCODE_S])
		_position -= forward * delta * _speed;

	if (state[SDL_SCANCODE_A])
		_position -= right * delta * _speed;
	if (state[SDL_SCANCODE_D])
		_position += right * delta * _speed;

	
	if (state[SDL_SCANCODE_SPACE])
		_position += up * delta * _speed;
	if (state[SDL_SCANCODE_LCTRL])
		_position -= up * delta * _speed;

	
	_projection = glm::perspective(glm::radians(_fov), (float)_width / (float)_height, 0.1f, 20.0f);
	_view = glm::lookAt(_position, _position + forward, up);
	glViewport(0, 0, _width, _height);
}
