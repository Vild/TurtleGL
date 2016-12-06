#pragma once

#include <cstdio>
#include <string>
#include <GL/glew.h>
#include "scopeexit.h"

enum class ShaderType {
	vertex = GL_SHADER_VERTEX,
	fragment = GL_SHADER_FRAGMENT
};

class ShaderException : Exception {};
class ShaderCreateException : ShaderException {};
	
class Shader {
public:

	void attach(const char * file, ShaderType type);
private:
	GLuint _vert = 0;
	GLuint _frag = 0;
	Gluint _program = 0;
}
