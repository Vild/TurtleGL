#pragma once

#include <SDL2/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glew.h>
#include <string>

class Texture {
public:
	Texture(const std::string& texture);
	Texture(GLuint texture);
	virtual ~Texture();

	void bind(int slot);

	GLuint getTexture();

private:
	GLuint _texture;
};
