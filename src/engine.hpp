#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <exception>
#include <memory>
#include <cmath>

#include "scopeexit.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "framebuffer.hpp"
#include "texture.hpp"
#include "entity/box.hpp"

class Engine {
public:
	Engine();
	virtual ~Engine();

	int run();

private:
	uint32_t _width = 1280;
	uint32_t _height = 720;

	float _speed = 2.5f;
	float _fov = 45.0f;

	float _yaw = 0; // +Z (at 0, 0, 0)
	float _pitch = 0.0f;

	glm::vec3 _position = glm::vec3(0, 0, -2);

	glm::mat4 _projection;
	glm::mat4 _view;

	bool _quit;
	SDL_Window* _window;
	SDL_GLContext _context;

	std::shared_ptr<ShaderProgram> _baseProgram; // The base shader for everything
	std::shared_ptr<Texture> _brickTexture;
	std::shared_ptr<Box> _box;
	std::shared_ptr<Mesh> _sphere;


	// Deferred stuff
	std::shared_ptr<ShaderProgram> _deferredProgram;
	std::shared_ptr<Mesh> _deferredPlane;

	std::shared_ptr<Framebuffer> _screen;
	std::shared_ptr<Framebuffer> _deferred;

	static const int LIGHT_COUNT = 16;
	glm::vec3 _lightsPos[LIGHT_COUNT];
	glm::vec3 _lightsColor[LIGHT_COUNT];
	std::shared_ptr<Box> _lightCube;
	std::shared_ptr<ShaderProgram> _lightProgram;

	void _initSDL();
	void _initGL();
	void _initShaders();
	void _initMeshes();
	void _initFramebuffers();
	void _initLights();

	void _resolutionChanged();
	void _updateMovement(float delta, bool updateCamera);
};
