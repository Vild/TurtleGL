#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <exception>
#include <memory>

#include "scopeexit.hpp"
#include "shader.hpp"
#include "mesh.hpp"

class Engine {
public:
	Engine();
	~Engine();

	int run();	
private:
	bool _quit;
	SDL_Window * _window;
	SDL_GLContext _context;

	std::shared_ptr<ShaderProgram> _baseProgram; // The base shader for everything
	std::shared_ptr<Mesh> _mesh;

	void _initSDL();
	void _initGL();
	void _initShaders();
	void _initMeshes();
};

