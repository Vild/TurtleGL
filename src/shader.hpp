#pragma once

#include <cstdio>
#include <string>
#include <GL/glew.h>
#include "scopeexit.hpp"
#include <exception>

enum class ShaderType {
	vertex = GL_VERTEX_SHADER,
	fragment = GL_FRAGMENT_SHADER
};

class ShaderException : public std::exception {};
class ShaderCreateException : public ShaderException {};
	
class Shader {
public:

	bool attach(const char * file, ShaderType type);
private:
	GLuint _vert = 0;
	GLuint _frag = 0;
	GLuint _program = 0;
};
