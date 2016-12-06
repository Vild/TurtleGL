#include "shader.hpp"

bool Shader::attach(const char * file, ShaderType type) {
	FILE * fp = fopen(file, "rb");
	if (!fp)
		return false;
	SCOPE_EXIT(fclose(fp););

	fseek(fp, SEEK_END, SEEK_SET);
	long size = ftell(fp);

	fseek(fp, SEEK_SET, 0);

	char * str = (char*)malloc(size + 1);
	SCOPE_EXIT(delete str);
	str[size] = '\0';

	fread(str, size, 1, fp);

	GLuint shader = glCreateShader((GLenum)type);
	glShaderSource(shader, 1, &str, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        
		GLchar error[0x1000];
		glGetShaderInfoLog(shader, len, NULL, error);
        
		const char * strtype;
		switch(type) {
		case ShaderType::vertex: strtype = "vertex"; break;
		case ShaderType::fragment: strtype = "geometry"; break;
		default: strtype = "(unknown)"; break;
		}

		printf("Compile failure in %s shader:\n%s\n", strtype, error);
		return false;
	}
	return true;
}

