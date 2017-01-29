#pragma once

#include <cstdio>
#include <string>
#include <GL/glew.h>
#include <exception>
#include <vector>
#include <memory>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "scopeexit.hpp"

enum class ShaderType {
	vertex = GL_VERTEX_SHADER,		 //
	fragment = GL_FRAGMENT_SHADER, //
	geometry = GL_GEOMETRY_SHADER	//
};

class ShaderException : public std::exception {
public:
	ShaderException(const std::string& msg) { _what = msg; }

	const char* what() const noexcept { return _what.c_str(); }

private:
	std::string _what;
};

class ShaderUnitException : public ShaderException {
public:
	ShaderUnitException(const std::string& msg) : ShaderException(msg) {}
};

class ShaderProgramException : public ShaderException {
public:
	ShaderProgramException(const std::string& msg) : ShaderException(msg) {}
};

class ShaderUnit {
public:
	ShaderUnit(const std::string& file, ShaderType type);
	virtual ~ShaderUnit();

	inline GLuint getUnit() const { return _unit; }

private:
	GLuint _unit = GL_FALSE;
};

class ShaderProgram {
public:
	ShaderProgram();
	virtual ~ShaderProgram();

	ShaderProgram& attach(std::shared_ptr<ShaderUnit> unit);

	void finalize();

	ShaderProgram& addUniform(const std::string& name);

	ShaderProgram& bind();

	GLint getAttribute(const std::string& name) const;

	ShaderProgram& setUniform(const std::string& name, const glm::vec3& value);
	ShaderProgram& setUniform(const std::string& name, const glm::vec3* values, GLuint count);
	ShaderProgram& setUniform(const std::string& name, const glm::mat4& value);
	ShaderProgram& setUniform(const std::string& name, int value);

private:
	GLuint _program = GL_FALSE;
	std::vector<std::shared_ptr<ShaderUnit>> _units;
	std::map<std::string, GLint> _uniform;
};
