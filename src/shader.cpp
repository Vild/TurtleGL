#include "shader.hpp"

ShaderUnit::ShaderUnit(const std::string & file, ShaderType type) {
	FILE * fp = fopen(file.c_str(), "rb");
	if (!fp)
		throw ShaderUnitException("File not found!");
	SCOPE_EXIT(fclose(fp););

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0, SEEK_SET);

	char * str = (char*)malloc(size + 1);
	SCOPE_EXIT(delete str);
	str[size] = '\0';

	fread(str, size, 1, fp);

	_unit = glCreateShader((GLenum)type);
	glShaderSource(_unit, 1, &str, NULL);
	glCompileShader(_unit);

	GLint status;
	glGetShaderiv(_unit, GL_COMPILE_STATUS, &status);
	/*if (status == GL_FALSE)*/ {
		GLint len;
		glGetShaderiv(_unit, GL_INFO_LOG_LENGTH, &len);
        
		std::vector<GLchar> errorLog(len);
		glGetShaderInfoLog(_unit, len, &len, errorLog.data());
        
		const char * strtype;
		switch(type) {
		case ShaderType::vertex: strtype = "vertex"; break;
		case ShaderType::fragment: strtype = "fragment"; break;
		case ShaderType::geometry: strtype = "geometry"; break;
		default: strtype = "(unknown)"; break;
		}

		char buf[0x1000];
		snprintf(buf, sizeof(buf), "Compile %s in %s(%s) shader:\n%s\n", status == GL_FALSE ? "failure" : "successful", file.c_str(), strtype, errorLog.data());
		if (status == GL_FALSE)
			throw ShaderUnitException(std::string(buf));
		else
			printf(buf);
	}
}

ShaderUnit::~ShaderUnit() {
	glDeleteShader(_unit);
}

ShaderProgram::ShaderProgram() {
	_program = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
	while (_units.size()) {
		glDetachShader(_program, _units.back()->getUnit());
		_units.pop_back();
	}
	glDeleteProgram(_program);
}

ShaderProgram & ShaderProgram::attach(std::shared_ptr<ShaderUnit> unit) {
	glAttachShader(_program, unit->getUnit());
	_units.push_back(unit);
	return *this;
}

void ShaderProgram::finalize() {
	glBindFragDataLocation(_program, 0, "outColor");
	
	glLinkProgram(_program);
	GLint status;
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint len;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &len);

		std::vector<GLchar> errorLog(len);
		glGetProgramInfoLog(_program, len, &len, errorLog.data());

		char buf[0x1000];
		snprintf(buf, sizeof(buf), "Linking failed (%d):\n%s", _program, errorLog.data());
		throw ShaderUnitException(std::string(buf));
	}
}

ShaderProgram & ShaderProgram::addUniform(const std::string & name) {
	GLint loc = glGetUniformLocation(_program, name.c_str());
	if (loc == -1) {
		char buf[0x1000];
		snprintf(buf, sizeof(buf), "Uniform not found: %s", name.c_str());
		throw ShaderProgramException(std::string(buf));
	}

	_uniform[name] = loc;
	return *this;
}

void ShaderProgram::bind() const {
	glUseProgram(_program);
}

GLint ShaderProgram::getAttribute(const std::string & name) const {
	return glGetAttribLocation(_program, name.c_str());
}

ShaderProgram & ShaderProgram::setUniform(const std::string & name, const glm::vec3 & value) {
	glUniform3fv(_uniform[name], 1, glm::value_ptr(value));
	return *this;
}

ShaderProgram & ShaderProgram::setUniform(const std::string & name, const glm::mat4 & value) {
	glUniformMatrix4fv(_uniform[name], 1, GL_FALSE, glm::value_ptr(value));
	return *this;
}

ShaderProgram & ShaderProgram::setUniform(const std::string & name, int value) {
	glUniform1i(_uniform[name], value);
	return *this;
}
