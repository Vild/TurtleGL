#include "engine.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

Engine::Engine() {
	_initSDL();
	_initGL();
	_initShaders();
	_initMeshes();
	_initFramebuffers();
	_initLights();
}

Engine::~Engine() {
	IMG_Quit();

	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

int Engine::run() {
	_quit = false;
	int fps = 0;
	uint32_t lastTime = SDL_GetTicks();

	_resolutionChanged();
	_updateMovement(0, false);
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
					SDL_WarpMouseInWindow(_window, _width / 2, _height / 2);
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
					_resolutionChanged();
				}
				break;
			default:
				break;
			}
		}

		uint32_t curTime = SDL_GetTicks();
		float delta = (curTime - lastTime) / 1000.0f;
		lastTime = curTime;
		_updateMovement(delta, updateCamera);

		// Render step 1 - Render everything to deferredFB
		_deferred->bind();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_baseProgram->bind();
		_brickTexture->bind(0);
		_baseProgram->setUniform("brickTex", 0);
		_box->getTranslation() *= glm::rotate(delta, glm::vec3(0, -1.5 * 10, 0));

		for (int y = 0; y < 3; y++)
			for (int i = 0; i < 3 * 3; i++) {
				glm::mat4 model = glm::translate(glm::vec3{(i % 3) * 4, y * 4, (i / 3) * 4}) * _box->getTranslation();
				glm::mat4 mvp = _projection * _view * model;
				_baseProgram->setUniform("m", model);
				_box->render(mvp);
			}
		glm::mat4 model = glm::translate(glm::vec3(0,0,0)) * 
			glm::rotate(delta, glm::vec3(0, -1.5 * 10, 0)) * 
			glm::scale(glm::vec3(5.0f, 5.0f, 5.0f));
		_baseProgram->setUniform("m", model);
		_sphere->render(_projection * _view * model);

		// Render step 2 - Render to screen
		_screen->bind();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_deferredProgram->bind();
		_deferred->getAttachments()[0].texture->bind(0);
		_deferred->getAttachments()[1].texture->bind(1);
		_deferred->getAttachments()[2].texture->bind(2);
		_deferredProgram->setUniform("defPos", 0).setUniform("defNormal", 1).setUniform("defDiffuseSpecular", 2);

		_deferredPlane->render(glm::mat4(1));

		// Render step 3 - Render lightsources
		// Render step 3.1 - Move deferred depth buffer to screen
		_deferred->bind(true, false);
		_screen->bind(true, false);
		glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		_screen->bind();

		// Render step 3.2 - Render lightsources as cubes
		_lightProgram->bind();
		for (int i = 0; i < LIGHT_COUNT; i++) {
			_lightProgram->setUniform("lightColor", _lightsColor[i]);
			glm::mat4 model = glm::translate(_lightsPos[i]) * _lightCube->getTranslation();
			glm::mat4 mvp = _projection * _view * model;
			_baseProgram->setUniform("m", model);
			_lightCube->render(mvp);
		}

		fps++;
		SDL_GL_SwapWindow(_window);
	}
	return 0;
}

void Engine::_initSDL() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		throw "SDL could not be inited";

	_window = SDL_CreateWindow("TurtleGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width, _height,
														 SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
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

	glViewport(0, 0, _width, _height);

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
		throw "Failed to load SDL_Image";
}

void Engine::_initShaders() {
	{
		_baseProgram = std::make_shared<ShaderProgram>();
		_baseProgram->attach(std::make_shared<ShaderUnit>("assets/shaders/base.vert", ShaderType::vertex))
			.attach(std::make_shared<ShaderUnit>("assets/shaders/base.frag", ShaderType::fragment))
			.finalize();
		_baseProgram->addUniform("mvp").addUniform("m").addUniform("brickTex");
	}
	{
		_deferredProgram = std::make_shared<ShaderProgram>();
		_deferredProgram->attach(std::make_shared<ShaderUnit>("assets/shaders/base.vert", ShaderType::vertex))
			.attach(std::make_shared<ShaderUnit>("assets/shaders/deferred.frag", ShaderType::fragment))
			.finalize();
		_deferredProgram->addUniform("mvp")
			.addUniform("defPos")
			.addUniform("defNormal")
			.addUniform("defDiffuseSpecular")
			.addUniform("lightsPos")
			.addUniform("lightsColor")
			.addUniform("cameraPos");
	}
}

void Engine::_initMeshes() {
	{ _box = std::make_shared<Box>(_baseProgram); }
	{
		std::vector<Vertex> verticies = {
			Vertex{glm::vec3{-1, 1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {0, 1}},	//
			Vertex{glm::vec3{1, 1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {1, 1}},		//
			Vertex{glm::vec3{1, -1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {1, 0}},	//
			Vertex{glm::vec3{-1, -1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {0, 0}}, //
		};
		std::vector<GLuint> indicies = {0, 2, 1, 2, 0, 3};
		_deferredPlane = std::make_shared<Mesh>(_deferredProgram, verticies, indicies);
		_sphere = std::make_shared<Mesh>(_baseProgram, "assets/objects/sphere_blue_blinn_760_tris_TRIANGULATED.obj");
		_brickTexture = std::make_shared<Texture>("assets/textures/brick.png");
	}
}

void Engine::_initFramebuffers() {
	_screen = std::make_shared<Framebuffer>(0);
	_deferred = std::make_shared<Framebuffer>();
	_deferred->bind()
		.attachTexture(0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, 3) // Position
		.attachTexture(1, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, 3) // Normal
		.attachTexture(2, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, 4) // Diffuse + Specular
		.attachRenderBuffer(_width, _height)
		.finalize();
}

void Engine::_initLights() {
	for (int i = 0; i < LIGHT_COUNT; i++)
		_lightsPos[i] = glm::vec3{(i / 4) * 4, ((i % 4) - (LIGHT_COUNT / 2)) * 4, -5};
	for (int i = 0; i < LIGHT_COUNT; i++)
		_lightsColor[i] = glm::vec3{(i % 4) / 4.0, 1, (i / 4) / 4.0};

	_deferredProgram->bind().setUniform("lightsPos", _lightsPos, LIGHT_COUNT).setUniform("lightsColor", _lightsColor, LIGHT_COUNT);

	_lightProgram = std::make_shared<ShaderProgram>();
	_lightProgram = std::make_shared<ShaderProgram>();
	_lightProgram->attach(std::make_shared<ShaderUnit>("assets/shaders/base.vert", ShaderType::vertex))
		.attach(std::make_shared<ShaderUnit>("assets/shaders/light.frag", ShaderType::fragment))
		.finalize();
	_lightProgram->addUniform("mvp").addUniform("m").addUniform("lightColor");

	_lightCube = std::make_shared<Box>(_lightProgram);
}

void Engine::_resolutionChanged() { // TODO: don't call all the time
	_projection = glm::perspective(glm::radians(_fov), (float)_width / (float)_height, 0.1f, 60.0f);
	glViewport(0, 0, _width, _height);
	_initFramebuffers();
}

void Engine::_updateMovement(float delta, bool updateCamera) { // TODO: don't call all the time
	if (updateCamera) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		SDL_WarpMouseInWindow(_window, _width / 2, _height / 2);

		x = _width / 2 - x;
		y = _height / 2 - y;

		float mspeed = 0.005f;
		_yaw += mspeed * x;
		_pitch += mspeed * y;
	}
	glm::vec3 forward(cos(_pitch) * sin(_yaw), sin(_pitch), cos(_pitch) * cos(_yaw));
	glm::vec3 right(sin(_yaw - M_PI / 2.0f), 0, cos(_yaw - M_PI / 2.0f));
	glm::vec3 up = glm::cross(right, forward);

	const uint8_t* state = SDL_GetKeyboardState(NULL);

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

	_view = glm::lookAt(_position, _position + forward, up);
	_deferredProgram->bind().setUniform("cameraPos", _position);
}
