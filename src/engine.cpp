#include "engine.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <cmath>

#include "entity/box.hpp"
#include "entity/sphere.hpp"
#include "entity/duck.hpp"

Engine::~Engine() {
	IMG_Quit();

	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

int Engine::run() {
	_init();
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
					_speed = 10.0f;
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
		glm::mat4 vp = _projection * _view;

		// Render step 1 - Render everything to deferredFB
		_deferred->bind();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_baseProgram->bind();
		for (std::shared_ptr<Entity> entity : _entities)
			entity->update(delta);

		_baseProgram->setUniform("vp", vp);
		for (std::shared_ptr<Entity> entity : _entities)
			entity->render();

		// Render step 2 - Render to screen
		_screen->bind();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_deferredProgram->bind();
		_deferred->getAttachments()[0].texture->bind(0);
		_deferred->getAttachments()[1].texture->bind(1);
		_deferred->getAttachments()[2].texture->bind(2);
		_deferredProgram->setUniform("defPos", 0).setUniform("defNormal", 1).setUniform("defDiffuseSpecular", 2);

		//_deferredPlane->uploadBufferArray("m", glm::mat4(1))); // Not needed
		_deferredProgram->setUniform("vp", glm::mat4(1));
		_deferredPlane->render();

		// Render step 3 - Render lightsources
		// Render step 3.1 - Move deferred depth buffer to screen
		_deferred->bind(true, false);
		_screen->bind(false, true);
		glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		_screen->bind();

		// Render step 3.2 - Render lightsources as cubes
		_lightProgram->bind();
		_lightBulb->uploadBufferArray("m", _lightsMatrix);
		//_lightBulb->uploadBufferArray("lightColor", _lightsColor);
		_lightProgram->setUniform("vp", vp);
		_lightBulb->render(_lightsMatrix.size(), GL_LINES);

		_skyboxProgram->bind();
		{
			auto kd = _skybox->getMaterial().map_Kd;
			if (kd) {
				_skybox->getMaterial().map_Kd->bind(0);
				_skyboxProgram->setUniform("diffuseTexture", 0);
			}
		}
		{
			glDisable(GL_CULL_FACE);
			glDepthFunc(GL_LEQUAL);
			glm::mat4 skyboxVP = _projection * glm::extractMatrixRotation(_view); // Never move the skybox only rotate
			_skyboxProgram->setUniform("vp", skyboxVP);
			_skybox->render();
			glEnable(GL_CULL_FACE);
		}

		fps++;
		SDL_GL_SwapWindow(_window);
	}
	return 0;
}

std::shared_ptr<TextureManager> Engine::getTextureManager() {
	return _textureManager;
}

void Engine::_init() {
	_initSDL();
	_initGL();
	_textureManager = std::make_shared<TextureManager>(); // TODO: Move to own function?
	_initShaders();
	_initMeshes();
	_initFramebuffers();
	_initLights();
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

	SDL_GL_SetSwapInterval(_vsync);
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
		_baseProgram->bind().addUniform("vp").addUniform("diffuseTexture");
		_baseProgram->setUniform("diffuseTexture", 0);
	}
	{
		_skyboxProgram = std::make_shared<ShaderProgram>();
		_skyboxProgram->attach(std::make_shared<ShaderUnit>("assets/shaders/skybox.vert", ShaderType::vertex))
			.attach(std::make_shared<ShaderUnit>("assets/shaders/skybox.frag", ShaderType::fragment))
			.finalize();
		_skyboxProgram->bind().addUniform("vp").addUniform("diffuseTexture");
	}
	{
		_deferredProgram = std::make_shared<ShaderProgram>();
		_deferredProgram->attach(std::make_shared<ShaderUnit>("assets/shaders/base.vert", ShaderType::vertex))
			.attach(std::make_shared<ShaderUnit>("assets/shaders/deferred.frag", ShaderType::fragment))
			.finalize();
		_deferredProgram->bind().addUniform("vp")
			.addUniform("defPos")
			.addUniform("defNormal")
			.addUniform("defDiffuseSpecular")
			.addUniform("cameraPos");
	}
}

void Engine::_initMeshes() {
	{
		_skybox = std::make_shared<Mesh>(_skyboxProgram, "assets/objects/skybox.obj");
		_skybox
			->addBuffer("m",
									[](std::shared_ptr<ShaderProgram> program, GLuint id) {
										GLint m = program->getAttribute("m");
										if (m == -1)
											return;
										glm::mat4 mData = glm::scale(glm::vec3(1.0f));

										glBindBuffer(GL_ARRAY_BUFFER, id);
										glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(mData), GL_STATIC_DRAW); // Will only be uploaded once

										for (int i = 0; i < 4; i++) {
											glEnableVertexAttribArray(m + i);
											glVertexAttribPointer(m + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4) * i));
											glVertexAttribDivisor(m + i, 1);
										}

										glBindBuffer(GL_ARRAY_BUFFER, 0);
									})
			.finalize();
	}
	_entities.push_back(std::make_shared<Box>(_baseProgram));
	_entities.push_back(std::make_shared<Sphere>(_baseProgram));
	_entities.push_back(std::make_shared<Duck>(_baseProgram));
	{
		std::vector<Vertex> verticies = {
			Vertex{glm::vec3{-1, 1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {0, 1}},	//
			Vertex{glm::vec3{1, 1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {1, 1}},		//
			Vertex{glm::vec3{1, -1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {1, 0}},	//
			Vertex{glm::vec3{-1, -1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {0, 0}}, //
		};
		std::vector<GLuint> indicies = {0, 2, 1, 2, 0, 3};
		_deferredPlane = std::make_shared<Mesh>(_deferredProgram, verticies, indicies);
		_deferredPlane
			->addBuffer("m",
									[](std::shared_ptr<ShaderProgram> program, GLuint id) {
										GLint m = program->getAttribute("m");
										if (m == -1)
											return;

										glm::mat4 mData = glm::mat4(1);

										glBindBuffer(GL_ARRAY_BUFFER, id);
										glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(mData), GL_STATIC_DRAW); // Will only be uploaded once

										for (int i = 0; i < 4; i++) {
											glEnableVertexAttribArray(m + i);
											glVertexAttribPointer(m + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4) * i));
											glVertexAttribDivisor(m + i, 1);
										}

										glBindBuffer(GL_ARRAY_BUFFER, 0);
									})
			.finalize();
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
	_lights.resize(LIGHT_COUNT);
//	for (int i = 0; i < LIGHT_COUNT; i++) {
//		_lights[i].pos = glm::vec3{((i % 8) % 3) * 6 - 2, 2 + (i / 8) * 4, ((i % 8) / 3) * 6 - 2};
//		float r = (i % 4) / 4.0;
//		float g = (i / 4) / 4.0;
//		float b = 1 - r - g;
//		if (b < 0)
//			b = 0;
//
//		_lights[i].color = glm::vec3{r, g, b};
//
//
//// https://learnopengl.com/#!Advanced-Lighting/Deferred-Shading
//		GLfloat constant = 1.0;
//		_lights[i].linear = 0.7;
//		_lights[i].quadratic = 1.8;
//		GLfloat lightMax = fmaxf(fmaxf(_lights[i].color.r, _lights[i].color.g), _lights[i].color.b);
//		_lights[i].radius = (-_lights[i].linear + sqrtf(_lights[i].linear * _lights[i].linear - 4 * _lights[i].quadratic * (constant - (256.0 / 5.0) * lightMax))) / (2 * _lights[i].quadratic);
//	}

	// One light for testing. 
	_lights[0].pos = glm::vec3(10, 10, 10);
	_lights[0].color = glm::vec3( 1, 1, 1 );
	GLfloat constant = 1.0;
	_lights[0].linear = 0.7;
	_lights[0].quadratic = 1.8;
	GLfloat lightMax = fmaxf(fmaxf(_lights[0].color.r, _lights[0].color.g), _lights[0].color.b);
	_lights[0].radius = (-_lights[0].linear + sqrtf(_lights[0].linear * _lights[0].linear - 4 * _lights[0].quadratic * (constant - (256.0 / 5.0) * lightMax))) / (2 * _lights[0].quadratic);


	_lightsBuffer = std::make_shared<UniformBuffer>(sizeof(Light) * LIGHT_COUNT);
	_lightsBuffer->setDataRaw(&_lights[0], sizeof(Light) * LIGHT_COUNT);

	_deferredProgram->bind().addUniformBuffer("Lights", _lightsBuffer, 8);

	_lightProgram = std::make_shared<ShaderProgram>();
	_lightProgram->attach(std::make_shared<ShaderUnit>("assets/shaders/light.vert", ShaderType::vertex))
		.attach(std::make_shared<ShaderUnit>("assets/shaders/light.frag", ShaderType::fragment))
		.finalize();
	_lightProgram->bind().addUniform("vp").addUniformBuffer("Lights", _lightsBuffer, 8);

	_lightsMatrix.resize(LIGHT_COUNT);
	for (int i = 0; i < LIGHT_COUNT; i++)
		_lightsMatrix[i] = glm::scale(glm::translate(_lights[i].pos), glm::vec3(_lights[i].radius / 2.0));

	_lightBulb = std::make_shared<Mesh>(_lightProgram, "assets/objects/sphere_blue_blinn_760_tris_TRIANGULATED.obj");

	_lightBulb
		->addBuffer("m",
								[&](std::shared_ptr<ShaderProgram> program, GLuint id) {
									GLint m = program->getAttribute("m");
									if (m == -1)
										return;
									glBindBuffer(GL_ARRAY_BUFFER, id);
									// GL_DYNAMIC_DRAW because the data will be changed alot
									glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * LIGHT_COUNT, &_lightsMatrix[0], GL_DYNAMIC_DRAW);

									// Hack below because glVertexAttribPointer can't handle mat4.
									// Mat4 internally in the shader will be converted to a 4 vec4.
									for (int i = 0; i < 4; i++) {
										glEnableVertexAttribArray(m + i);
										glVertexAttribPointer(m + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4) * i));
										glVertexAttribDivisor(m + i, 1);
									}

									glBindBuffer(GL_ARRAY_BUFFER, 0);
								})
		/*.addBuffer("lightColor",
							 [&](std::shared_ptr<ShaderProgram> program, GLuint id) {
								 GLint lightColor = program->getAttribute("lightColor");
								 if (lightColor == -1)
									 return;

								 glBindBuffer(GL_ARRAY_BUFFER, id);
								 glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * LIGHT_COUNT, &_lightsColor[0], GL_DYNAMIC_DRAW);

								 glEnableVertexAttribArray(lightColor);
								 glVertexAttribPointer(lightColor, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
								 glVertexAttribDivisor(lightColor, 1);

								 glBindBuffer(GL_ARRAY_BUFFER, 0);
							 })*/
		.finalize();
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
		_pitch = glm::clamp(_pitch, (float)-M_PI / 2, (float)M_PI / 2);
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
		_position += glm::vec3(0, 1, 0) * delta * _speed;
	if (state[SDL_SCANCODE_LCTRL])
		_position -= glm::vec3(0, 1, 0) * delta * _speed;

	_view = glm::lookAt(_position, _position + forward, up);
	_deferredProgram->bind().setUniform("cameraPos", _position);
}
