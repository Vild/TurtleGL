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

enum ShaderAttributeID : GLint {
	vertPos = 0,
	vertNormal,
	vertColor,
	vertUV,
	vertTangent,
	m, // m = mat4, Requires 4 spaces
	_m_priv_1,
	_m_priv_2,
	_m_priv_3
};

enum class ShaderType {
	vertex = GL_VERTEX_SHADER,		 //
	fragment = GL_FRAGMENT_SHADER, //
	geometry = GL_GEOMETRY_SHADER, //
	compute = GL_COMPUTE_SHADER		 //
};

inline const char* toString(ShaderType shader) {
	switch (shader) {
	case ShaderType::vertex:
		return "Vertex";
	case ShaderType::fragment:
		return "Fragment";
	case ShaderType::geometry:
		return "Geometry";
	case ShaderType::compute:
		return "Compute";
	default:
		return "[Unknown ShaderType]";
	}
}

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
	//XXX: amdHack, Last minute hack to make shader work on AMD hardware
	ShaderUnit(const std::string& file, ShaderType type, bool amdHack = false);
	virtual ~ShaderUnit();

	inline GLuint getUnit() const { return _unit; }

private:
	GLuint _unit = GL_FALSE;
};

class UniformBuffer {
public:
	UniformBuffer(size_t size, GLenum dataMode = GL_STATIC_DRAW);
	~UniformBuffer();

	template <typename T>
	void setData(const T& value) {
		glBindBuffer(GL_UNIFORM_BUFFER, _bufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &value);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	template <typename T>
	void setDataArray(const std::vector<T>& values) {
		glBindBuffer(GL_UNIFORM_BUFFER, _bufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T) * values.size(), &values[0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void setDataRaw(void* value, size_t size) {
		glBindBuffer(GL_UNIFORM_BUFFER, _bufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, value);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	GLuint getID() const { return _bufferID; }

private:
	GLuint _bufferID;
};

class ShaderProgram {
public:
	ShaderProgram();
	virtual ~ShaderProgram();

	ShaderProgram& attach(std::shared_ptr<ShaderUnit> unit);

	void finalize();

	ShaderProgram& addUniform(const std::string& name);
	/// XXX: bindingID start at 0?
	ShaderProgram& addUniformBuffer(const std::string& name, std::shared_ptr<UniformBuffer> buffer, GLuint bindingID);

	ShaderProgram& bind();
	ShaderProgram& compute(const glm::ivec3& groups); /// Only valid for compute shaders

	template <typename T>
	ShaderProgram& setUniform(const std::string& name, const T& value) {
		try {
			_glUniform(_uniform.at(name), 1, &value);
		} catch (std::out_of_range& e) { //-V565
																		 // std::cerr << "Uniform is missing! Did you forget to use that variable in that shader?: " << name << std::endl;
			// throw ShaderProgramException(std::string("Uniform is missing! Did you forget to use that variable in that shader?: ") + name);
		}
		return *this;
	}

	ShaderProgram& setUniform(const std::string& name, const bool& value) {
		try {
			GLint i = value;
			_glUniform(_uniform.at(name), 1, &i);
		} catch (std::out_of_range& e) { //-V565
																		 // std::cerr << "Uniform is missing! Did you forget to use that variable in that shader?: " << name << std::endl;
			// throw ShaderProgramException(std::string("Uniform is missing! Did you forget to use that variable in that shader?: ") + name);
		}
		return *this;
	}

	template <typename T>
	ShaderProgram& setUniformArray(const std::string& name, const std::vector<T>& values) {
		try {
			_glUniform(_uniform.at(name), values.size(), &values[0]);
		} catch (std::out_of_range& e) { //-V565
																		 // std::cerr << "Uniform is missing! Did you forget to use that variable in that shader?: " << name << std::endl;
			// throw ShaderProgramException(std::string("Uniform is missing! Did you forget to use that variable in that shader?: ") + name);
		}
		return *this;
	}

private:
	GLuint _program = GL_FALSE;
	std::vector<std::shared_ptr<ShaderUnit>> _units;
	std::map<std::string, GLint> _uniform;
	std::map<std::string, std::shared_ptr<UniformBuffer>> _uniformBuffer;

	static void _glUniform(GLuint id, GLuint count, const GLfloat* data) { glUniform1fv(id, count, data); };
	static void _glUniform(GLuint id, GLuint count, const GLdouble* data) { glUniform1dv(id, count, data); };
	static void _glUniform(GLuint id, GLuint count, const GLint* data) { glUniform1iv(id, count, data); };
	static void _glUniform(GLuint id, GLuint count, const GLuint* data) { glUniform1uiv(id, count, data); };

	static void _glUniform(GLuint id, GLuint count, const glm::vec2* data) { glUniform2fv(id, count, glm::value_ptr(data[0])); };
	static void _glUniform(GLuint id, GLuint count, const glm::vec3* data) { glUniform3fv(id, count, glm::value_ptr(data[0])); };
	static void _glUniform(GLuint id, GLuint count, const glm::vec4* data) { glUniform4fv(id, count, glm::value_ptr(data[0])); };

	static void _glUniform(GLuint id, GLuint count, const glm::mat2* data) { glUniformMatrix2fv(id, count, GL_FALSE, glm::value_ptr(data[0])); };
	static void _glUniform(GLuint id, GLuint count, const glm::mat3* data) { glUniformMatrix3fv(id, count, GL_FALSE, glm::value_ptr(data[0])); };
	static void _glUniform(GLuint id, GLuint count, const glm::mat4* data) { glUniformMatrix4fv(id, count, GL_FALSE, glm::value_ptr(data[0])); };
};
