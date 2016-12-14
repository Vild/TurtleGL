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

class Engine {
public:
	Engine();
	~Engine();

	int run();	
private:
	uint32_t _width = 1280;
	uint32_t _height = 720;

	float _speed = 2.5f;
	float _fov = 45.0f;
	
	float _yaw = M_PI; // -Z
	float _pitch = 0.0f;
	
	glm::vec3 _position = glm::vec3(0, 0, 2);
	
	glm::mat4 _projection;
	glm::mat4 _view;
	
	bool _quit;
	SDL_Window * _window;
	SDL_GLContext _context;

	std::shared_ptr<ShaderProgram> _baseProgram; // The base shader for everything
	std::shared_ptr<Mesh> _mesh;

	void _initSDL();
	void _initGL();
	void _initShaders();
	void _initMeshes();

	void _updateMatrices(float delta, bool updateCamera);
};

