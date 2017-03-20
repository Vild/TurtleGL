#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <exception>
#include <memory>
#include <cmath>

#include "gl/shader.hpp"
#include "gl/mesh.hpp"
#include "gl/gbuffer.hpp"
#include "gl/texture.hpp"
#include "io/texturemanager.hpp"
#include "entity/entity.hpp"

class Engine {
public:
	static Engine& getInstance() {
		static Engine instance;
		return instance;
	}

	Engine(Engine const&) = delete;
	void operator=(Engine const&) = delete;

	int run();

	std::shared_ptr<TextureManager> getTextureManager();

private:
	uint32_t _width = 1280;
	uint32_t _height = 720;
	bool _vsync = true;

	float _speed = 5.0f;
	float _fov = 80.0f;

	float _yaw = 0; // +Z (at 0, 0, 0)
	float _pitch = 0.0f;

	glm::vec3 _position = glm::vec3(0, 0, -2);

	glm::mat4 _projection;
	glm::mat4 _view;

	bool _quit;
	SDL_Window* _window;
	SDL_GLContext _context;

	std::shared_ptr<TextureManager> _textureManager;

	std::shared_ptr<ShaderProgram> _baseProgram;	 // The base shader for everything
	std::shared_ptr<ShaderProgram> _skyboxProgram; // Skybox program

	std::shared_ptr<Mesh> _skybox;

	std::vector<std::shared_ptr<Entity>> _entities;

	// Shadowmap stuff
	std::shared_ptr<ShaderProgram> _shadowmapProgram;
	std::shared_ptr<GBuffer> _shadowmapFBO;

	// Billboard Particle stuff
	std::shared_ptr<ShaderProgram> _particleProgram;
	std::shared_ptr<Entity> _particles;
	glm::vec3 cameraRightWorldSpace;
	glm::vec3 cameraUpWorldSpace;

	// Gaussian blur stuff
	std::shared_ptr<ShaderProgram> _gaussianProgram;
	std::shared_ptr<GBuffer> _gaussianFBO0;
	std::shared_ptr<GBuffer> _gaussianFBO1;
	std::shared_ptr<Mesh> _gaussianPlane;
	std::shared_ptr<ShaderProgram> _finalProgram;

	// Deferred stuff
	std::shared_ptr<ShaderProgram> _deferredProgram;
	std::shared_ptr<Mesh> _deferredPlane;

	std::shared_ptr<GBuffer> _screen;
	std::shared_ptr<GBuffer> _deferred;

	std::shared_ptr<ShaderProgram> _lightProgram;

	struct Light {
		glm::vec3 pos;
		float radius;
		glm::vec3 color;
		float linear;
		float quadratic;
		float yaw;
		float pitch;
		float _pad0;

		Light() : pos(glm::vec3(0)), radius(0), color(glm::vec3(0)), linear(0), quadratic(0), yaw(0), pitch(0), _pad0(0) {}
	};

	static const int LIGHT_COUNT = 1;
	std::vector<Light> _lights;
	std::shared_ptr<UniformBuffer> _lightsBuffer;

	std::shared_ptr<Mesh> _lightBulb;
	std::vector<glm::mat4> _lightsMatrix;

	glm::mat4 _lightMV;
	glm::mat4 _lightP;
	glm::mat4 _lightS;

	const int _shadowmapSize = 1024;

	// Settings (OpenGL)
	bool _setting_ogl_doBackFaceCulling = true;
	bool _setting_ogl_renderLights = true;
	bool _setting_ogl_doBlur = true;


	// Settings (Shaders)
	bool _setting_base_doBackFaceCulling = true;
	float _setting_base_defaultSpecular = 0.5f;

	bool _setting_deferred_enableAmbient = true;
	bool _setting_deferred_enableShadow = true;
	bool _setting_deferred_enableDiffuse = true;
	bool _setting_deferred_enableSpecular = true;
	float _setting_deferred_shininess = 64.0f;
	int _setting_deferred_pcfSamples = 8;

	bool _setting_filter_enableGaussian = true;
	int _setting_filter_samples = 2;

	Engine() {}
	virtual ~Engine();

	void _init();
	void _initSDL();
	void _initGL();
	void _initImGui();
	void _initShaders();
	void _initMeshes();
	void _initGBuffers();
	void _initLights();
	void _initBillboard();

	void _resolutionChanged();
	void _updateMovement(float delta, bool updateCamera);
	void _updateLights();
};
