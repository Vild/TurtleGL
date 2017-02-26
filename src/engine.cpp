// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "engine.hpp"

#include "lib/imgui.h"
#include "lib/imgui_impl_sdl_gl3.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <cmath>

#include "entity/box.hpp"
#include "entity/earth.hpp"
#include "entity/duck.hpp"
#include "entity/plane.hpp"
#include "entity/triangle.hpp"

Engine::~Engine() {
	IMG_Quit();

	ImGui_ImplSdlGL3_Shutdown();

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
			ImGui_ImplSdlGL3_ProcessEvent(&event);
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
					SDL_ShowCursor(0);
					SDL_WarpMouseInWindow(_window, _width / 2, _height / 2);
				}
				break;

			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_RIGHT) {
					updateCamera = false;
					SDL_ShowCursor(1);
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
		ImGui_ImplSdlGL3_NewFrame(_window);

		{
			ImGui::SetNextWindowPos(ImVec2(8, 8), ImGuiSetCond_Always);
			ImGui::SetNextWindowSize(ImVec2(384, 32), ImGuiSetCond_Once);
			ImGui::Begin("Info panel", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		{
			ImGui::SetNextWindowPos(ImVec2(8, 48), ImGuiSetCond_Once);
			ImGui::SetNextWindowSize(ImVec2(384, 512), ImGuiSetCond_Once);
			ImGui::SetNextWindowCollapsed(true, ImGuiSetCond_Once);
			ImGui::Begin("Settings Window");
			if (ImGui::CollapsingHeader("OpenGL")) {
				if (ImGui::Checkbox("Backface culling", &_setting_ogl_doBackFaceCulling)) {
					if (_setting_ogl_doBackFaceCulling)
						glEnable(GL_CULL_FACE);
					else
						glDisable(GL_CULL_FACE);
				}
				ImGui::Checkbox("Render lights", &_setting_ogl_renderLights);
			}

			if (ImGui::CollapsingHeader("Base Program")) {
				_baseProgram->bind();
				if (ImGui::Checkbox("Geometry Backface Culling", &_setting_base_doBackFaceCulling))
					_baseProgram->setUniform("setting_doBackFaceCulling", _setting_base_doBackFaceCulling);

				ImGui::Separator();

				if (ImGui::DragFloat("Default Specular", &_setting_base_defaultSpecular, 0.005, 0, 1))
					_baseProgram->setUniform("setting_defaultSpecular", _setting_base_defaultSpecular);
			}

			if (ImGui::CollapsingHeader("Deferred Program")) {
				_deferredProgram->bind();
				if (ImGui::Checkbox("Ambient", &_setting_deferred_enableAmbient))
					_deferredProgram->setUniform("setting_enableAmbient", _setting_deferred_enableAmbient);
				if (ImGui::Checkbox("Shadow", &_setting_deferred_enableShadow))
					_deferredProgram->setUniform("setting_enableShadow", _setting_deferred_enableShadow);
				if (ImGui::Checkbox("Diffuse", &_setting_deferred_enableDiffuse))
					_deferredProgram->setUniform("setting_enableDiffuse", _setting_deferred_enableDiffuse);
				if (ImGui::Checkbox("Specular", &_setting_deferred_enableSpecular))
					_deferredProgram->setUniform("setting_enableSpecular", _setting_deferred_enableSpecular);

				ImGui::Separator();

				if (ImGui::DragFloat("Shininess", &_setting_deferred_shininess, 0.1, 0.1, 256))
					_deferredProgram->setUniform("setting_shininess", _setting_deferred_shininess);
			}

			ImGui::Separator();

			if (ImGui::CollapsingHeader("Lights")) {
				ImGui::Columns(2);
				for (int i = 0; i < LIGHT_COUNT; i++) {
					const ImVec4 color = ImColor(_lights[i].color.x, _lights[i].color.y, _lights[i].color.z, 1.0);

					ImGui::Text((std::string("Light #") + std::to_string(i)).c_str());
					ImGui::NextColumn();

					ImGui::PushStyleColor(ImGuiCol_Button, color);
					if (ImGui::ColorButton(color))
						ImGui::OpenPopup((std::string("color") + std::to_string(i)).c_str());
					ImGui::PopStyleColor();

					if (ImGui::BeginPopup((std::string("color") + std::to_string(i)).c_str())) {
						ImGui::Text((std::string("Editing Light #") + std::to_string(i)).c_str());
						if (ImGui::ColorEdit3("##color", glm::value_ptr(_lights[i].color))) {
							_lightsBuffer->setDataRaw(&_lights[0], sizeof(Light) * LIGHT_COUNT);
						}

						if (ImGui::DragFloat3("##pos", glm::value_ptr(_lights[i].pos), 0.1)) {
							_lightsBuffer->setDataRaw(&_lights[0], sizeof(Light) * LIGHT_COUNT);
							for (int i = 0; i < LIGHT_COUNT; i++)
								_lightsMatrix[i] = glm::scale(glm::translate(_lights[i].pos), glm::vec3(0.5f));

							GLfloat near_plane = 1.0f, far_plane = 50.0f;
							glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
							glm::mat4 lightView = glm::lookAt(_lights[0].pos, glm::vec3(0, 0, 0), glm::vec3(1.0f, 0.0f, 0.0f));
							glm::mat4 lightSpaceMatrix = lightProjection * lightView;
							_shadowmapProgram->bind().setUniform("lightSpaceMatrix", lightSpaceMatrix);
							_deferredProgram->bind().setUniform("lightSpaceMatrix", lightSpaceMatrix);
						}

						if (ImGui::Button("Close"))
							ImGui::CloseCurrentPopup();

						ImGui::EndPopup();
					}

					ImGui::NextColumn();
				}

				ImGui::Columns(1);
			}
			ImGui::End();
		}

		uint32_t curTime = SDL_GetTicks();
		float delta = (curTime - lastTime) / 1000.0f;
		lastTime = curTime;
		_updateMovement(delta, updateCamera);
		glm::mat4 vp = _projection * _view;

		// Render step 1 - Render everything to shadowmapFBO

		_shadowmapFBO->bind();
		glViewport(0, 0, 1024, 1024);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (std::shared_ptr<Entity> entity : _entities) {
			entity->update(delta);
		}

		_shadowmapProgram->bind();
		for (std::shared_ptr<Entity> entity : _entities) {
			entity->render();
		}
		glViewport(0, 0, _width, _height);

		// Render step 2 - Render everything to deferredFB
		_deferred->bind();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_baseProgram->bind();
		_baseProgram->setUniform("vp", vp);
		for (std::shared_ptr<Entity> entity : _entities) {
			entity->render();
		}

		// Render step 3 - Render to screen
		_screen->bind();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_deferredProgram->bind();
		_deferred->getAttachments()[0].texture->bind(10);
		_deferred->getAttachments()[1].texture->bind(11);
		_deferred->getAttachments()[2].texture->bind(12);
		_shadowmapFBO->getAttachments()[0].texture->bind(13);
		_deferredProgram->setUniform("defPos", 10).setUniform("defNormal", 11).setUniform("defDiffuseSpecular", 12).setUniform("shadowMap", 13);

		_deferredProgram->setUniform("vp", glm::mat4(1));
		_deferredPlane->render();

		// Render step 3 - Render lightsources
		// Render step 3.1 - Move deferred depth buffer to screen
		_deferred->bind(true, false);
		_screen->bind(false, true);
		glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		_screen->bind();

		// Render step 3.2 - Render lightsources as cubes
		if (_setting_ogl_renderLights) {
			_lightProgram->bind();
			_lightBulb->uploadBufferArray("m", _lightsMatrix);
			_lightProgram->setUniform("vp", vp);
			_lightBulb->render(_lightsMatrix.size(), GL_LINES);
		}

		_skyboxProgram->bind();
		_skybox->getMaterial().map_Kd->bind(0);
		{
			if (_setting_ogl_doBackFaceCulling)
				glDisable(GL_CULL_FACE);
			glDepthFunc(GL_LEQUAL);
			glm::mat4 skyboxVP = _projection * glm::extractMatrixRotation(_view); // Never move the skybox only rotate
			_skyboxProgram->setUniform("vp", skyboxVP);
			_skybox->render();
			if (_setting_ogl_doBackFaceCulling)
				glEnable(GL_CULL_FACE);
		}

		ImGui::Render();
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
	_initImGui();
	_textureManager = std::make_shared<TextureManager>(); // TODO: Move to own function?
	_initShaders();
	_initMeshes();
	_initGBuffers();
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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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

void Engine::_initImGui() {
	ImGui_ImplSdlGL3_Init(_window);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("assets/fonts/DroidSans.ttf", 18.0f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.83f, 0.95f, 0.95f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.39f, 0.80f, 0.80f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.27f, 0.27f, 0.87f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.25f, 0.75f, 0.75f, 1.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.55f, 0.55f, 0.67f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.75f, 0.75f, 0.67f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.49f, 0.49f, 0.45f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.49f, 0.49f, 0.60f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.56f, 0.56f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.16f, 0.47f, 0.47f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.11f, 0.33f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.14f, 0.42f, 0.42f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.33f, 0.78f, 0.78f, 0.67f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.33f, 0.78f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.18f, 0.55f, 0.55f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.44f, 0.81f, 0.81f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.33f, 0.78f, 0.78f, 0.60f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.33f, 0.78f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.55f, 0.55f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.33f, 0.78f, 0.78f, 0.67f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.33f, 0.78f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.44f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.33f, 0.78f, 0.78f, 0.67f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.33f, 0.78f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.25f, 0.74f, 0.74f, 0.40f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.25f, 0.74f, 0.74f, 0.60f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.25f, 0.74f, 0.74f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.53f, 0.84f, 0.84f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.53f, 0.84f, 0.84f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.22f, 0.67f, 0.67f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.53f, 0.84f, 0.84f, 0.40f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.53f, 0.84f, 0.84f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.53f, 0.84f, 0.84f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 0.84f, 0.84f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.53f, 0.84f, 0.84f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 0.84f, 0.84f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.13f, 0.40f, 0.40f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.09f, 0.27f, 0.27f, 0.67f);
}

void Engine::_initShaders() {
	{
		_shadowmapProgram = std::make_shared<ShaderProgram>();
		_shadowmapProgram->attach(std::make_shared<ShaderUnit>("assets/shaders/shadowmap.vert", ShaderType::vertex))
			.attach(std::make_shared<ShaderUnit>("assets/shaders/shadowmap.frag", ShaderType::fragment))
			.finalize();
		_shadowmapProgram->bind().addUniform("lightSpaceMatrix");
	}
	{
		_baseProgram = std::make_shared<ShaderProgram>();
		_baseProgram->attach(std::make_shared<ShaderUnit>("assets/shaders/base.vert", ShaderType::vertex))
			.attach(std::make_shared<ShaderUnit>("assets/shaders/base.geom", ShaderType::geometry))
			.attach(std::make_shared<ShaderUnit>("assets/shaders/base.frag", ShaderType::fragment))
			.finalize();
		_baseProgram->bind()
			.addUniform("vp")
			.addUniform("cameraPos")
			.addUniform("diffuseTexture")
			.addUniform("normalTexture")
			.addUniform("setting_doBackFaceCulling")
			.addUniform("setting_defaultSpecular");
		_baseProgram->setUniform("diffuseTexture", 0)
			.setUniform("normalTexture", 1)
			.setUniform("setting_doBackFaceCulling", _setting_base_doBackFaceCulling)
			.setUniform("setting_defaultSpecular", _setting_base_defaultSpecular);
	}
	{
		_skyboxProgram = std::make_shared<ShaderProgram>();
		_skyboxProgram->attach(std::make_shared<ShaderUnit>("assets/shaders/skybox.vert", ShaderType::vertex))
			.attach(std::make_shared<ShaderUnit>("assets/shaders/skybox.frag", ShaderType::fragment))
			.finalize();
		_skyboxProgram->bind().addUniform("vp").addUniform("diffuseTexture").addUniform("normalTexture");
		_skyboxProgram->setUniform("diffuseTexture", 0).setUniform("normalTexture", 1);
	}
	{
		_deferredProgram = std::make_shared<ShaderProgram>();
		_deferredProgram->attach(std::make_shared<ShaderUnit>("assets/shaders/deferred.vert", ShaderType::vertex))
			.attach(std::make_shared<ShaderUnit>("assets/shaders/deferred.frag", ShaderType::fragment))
			.finalize();
		_deferredProgram->bind()
			.addUniform("vp")
			.addUniform("lightSpaceMatrix")
			.addUniform("defPos")
			.addUniform("defNormal")
			.addUniform("defDiffuseSpecular")
			.addUniform("shadowMap")
			.addUniform("cameraPos")
			.addUniform("normalTexture")
			.addUniform("setting_enableAmbient")
			.addUniform("setting_enableShadow")
			.addUniform("setting_enableDiffuse")
			.addUniform("setting_enableSpecular")
			.addUniform("setting_shininess");
		_deferredProgram->setUniform("normalTexture", 1)
			.setUniform("setting_enableAmbient", _setting_deferred_enableAmbient)
			.setUniform("setting_enableShadow", _setting_deferred_enableShadow)
			.setUniform("setting_enableDiffuse", _setting_deferred_enableDiffuse)
			.setUniform("setting_enableSpecular", _setting_deferred_enableSpecular)
			.setUniform("setting_shininess", _setting_deferred_shininess);
	}
}

void Engine::_initMeshes() {
	{
		_skybox = std::make_shared<Mesh>("assets/objects/skybox.obj");
		_skybox
			->addBuffer("m",
									[](GLuint id) {
										glm::mat4 mData = glm::scale(glm::vec3(1.0f));

										glBindBuffer(GL_ARRAY_BUFFER, id);
										glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(mData), GL_STATIC_DRAW); // Will only be uploaded once

										for (int i = 0; i < 4; i++) {
											glEnableVertexAttribArray(ShaderAttributeID::m + i);
											glVertexAttribPointer(ShaderAttributeID::m + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4) * i));
											glVertexAttribDivisor(ShaderAttributeID::m + i, 1);
										}

										glBindBuffer(GL_ARRAY_BUFFER, 0);
									})
			.finalize();
	}
	_entities.push_back(std::make_shared<Box>());
	_entities.push_back(std::make_shared<Earth>());
	_entities.push_back(std::make_shared<Duck>());
	_entities.push_back(std::make_shared<Plane>());
	_entities.push_back(std::make_shared<Triangle>());
	{
		std::vector<Vertex> verticies = {
			Vertex{glm::vec3{-1, 1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {0, 1}},	//
			Vertex{glm::vec3{1, 1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {1, 1}},		//
			Vertex{glm::vec3{1, -1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {1, 0}},	//
			Vertex{glm::vec3{-1, -1, 0}, glm::vec3{0, 0, -1}, {1.0, 1.0, 1.0}, {0, 0}}, //
		};
		std::vector<GLuint> indicies = {0, 2, 1, 2, 0, 3};
		_deferredPlane = std::make_shared<Mesh>(verticies, indicies);
		_deferredPlane
			->addBuffer("m",
									[](GLuint id) {
										glm::mat4 mData = glm::mat4(1);
										glBindBuffer(GL_ARRAY_BUFFER, id);
										glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(mData), GL_STATIC_DRAW); // Will only be uploaded once

										for (int i = 0; i < 4; i++) {
											glEnableVertexAttribArray(ShaderAttributeID::m + i);
											glVertexAttribPointer(ShaderAttributeID::m + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4) * i));
											glVertexAttribDivisor(ShaderAttributeID::m + i, 1);
										}

										glBindBuffer(GL_ARRAY_BUFFER, 0);
									})
			.finalize();
	}
}

void Engine::_initGBuffers() {
	_screen = std::make_shared<GBuffer>(0);
	_shadowmapFBO = std::make_shared<GBuffer>();
	_shadowmapFBO->bind().attachDepthTexture(0, 1024, 1024);
	_deferred = std::make_shared<GBuffer>();
	_deferred->bind()
		.attachTexture(0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, 3)	// Position
		.attachTexture(1, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, 3)	// Normal
		.attachTexture(2, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, 4) // Diffuse + Specular
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
	//		_lights[i].radius = (-_lights[i].linear + sqrtf(_lights[i].linear * _lights[i].linear - 4 * _lights[i].quadratic * (constant - (256.0 / 5.0) *
	// lightMax))) / (2 * _lights[i].quadratic);
	//	}

	// One light for testing.
	_lights[0].pos = glm::vec3(0, 12, 0);
	_lights[0].color = glm::vec3(1, 1, 1);
	GLfloat constant = 1.0;
	_lights[0].linear = 0.7;
	_lights[0].quadratic = 1.8;
	GLfloat lightMax = fmaxf(fmaxf(_lights[0].color.r, _lights[0].color.g), _lights[0].color.b);
	_lights[0].radius = (-_lights[0].linear + sqrtf(_lights[0].linear * _lights[0].linear - 4 * _lights[0].quadratic * (constant - (256.0 / 5.0) * lightMax))) /
											(2 * _lights[0].quadratic);

	GLfloat near_plane = 1.0f, far_plane = 50.0f;
	// glm::mat4 lightProjection = glm::perspective(glm::radians(_fov), (float)_width / (float)_height, 0.1f, 60.0f);
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(_lights[0].pos, glm::vec3(0, 0, 0), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	_shadowmapProgram->bind().setUniform("lightSpaceMatrix", lightSpaceMatrix);
	_deferredProgram->bind().setUniform("lightSpaceMatrix", lightSpaceMatrix);

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
		_lightsMatrix[i] = glm::scale(glm::translate(_lights[i].pos), glm::vec3(0.5f));

	_lightBulb = std::make_shared<Mesh>("assets/objects/sphere_blue_blinn_760_tris_TRIANGULATED.obj");

	_lightBulb
		->addBuffer("m",
								[&](GLuint id) {
									glBindBuffer(GL_ARRAY_BUFFER, id);
									// GL_DYNAMIC_DRAW because the data will be changed alot
									glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * LIGHT_COUNT, &_lightsMatrix[0], GL_DYNAMIC_DRAW);

									// Hack below because glVertexAttribPointer can't handle mat4.
									// Mat4 internally in the shader will be converted to a 4 vec4.
									for (int i = 0; i < 4; i++) {
										glEnableVertexAttribArray(ShaderAttributeID::m + i);
										glVertexAttribPointer(ShaderAttributeID::m + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4) * i));
										glVertexAttribDivisor(ShaderAttributeID::m + i, 1);
									}

									glBindBuffer(GL_ARRAY_BUFFER, 0);
								})
		.finalize();
}

void Engine::_resolutionChanged() { // TODO: don't call all the time
	_projection = glm::perspective(glm::radians(_fov), (float)_width / (float)_height, 0.1f, 60.0f);
	glViewport(0, 0, _width, _height);
	_initGBuffers();
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
	_baseProgram->bind().setUniform("cameraPos", _position);
	_deferredProgram->bind().setUniform("cameraPos", _position);
}
