#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <exception>

#include "scopeexit.hpp"

class Engine {
public:
	Engine();
	~Engine();

	int run();	
private:
	bool _quit;
	SDL_Window * _window;
	SDL_GLContext _context;

	void _initSDL();
	void _initGL();	
};

