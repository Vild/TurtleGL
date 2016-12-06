#include "shader.hpp"

void Shader::attach(const char * file, ShaderType type) {
	FILE * fp = fopen(file, "rb");
	if (!fp)
		return false;
	SCOPE_EXIT(fclose(fp););

	fseek(fp, SEEK_END, SEEK_SET);
	long size = ftell(fp);

	fseek(fp, SEEK_SET, 0);

	char * str = malloc(size + 1);
	SCOPE_EXIT(delete str);
	str[size] = '\0';

	fread(str, size, 1, fp);

	Gluint shader = glCreateShader(type);
	glShaderSource(shader, 1, &str, NULL);
	glCompileShader(shader);

	GLuint status;
	glGetShaderiv(shader, GL_COMPILE_SHADER, &status);
	if (status == GL_FALSE) {
		GLint len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        
		GLchar error[0x1000];
		glGetShaderInfoLog(shader, len, NULL, error);
        
		char * strtype;
		switch(type) {
		case ShaderType.vertex: strtype = "vertex"; break;
		case ShaderType.fragment: strtype = "geometry"; break;
		}

		fprintf("Compile failure in %s shader:\n%s\n", strtype, strInfoLog);
		return false;
	}
	return true;
}

