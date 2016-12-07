#pragma once

#include <GL/glew.h>
#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include "shader.hpp"

struct Vertex {
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec4 color;
};

class Mesh {
public:
	Mesh(std::shared_ptr<ShaderProgram> program, std::vector<Vertex> vertices, std::vector<GLuint> indices);
	~Mesh();

	void render(const glm::mat4 & mvp);
	//void render(std::vector<glm::mat4> mvp); // Instance rendering
	
private:
	std::shared_ptr<ShaderProgram> _program;
	std::vector<Vertex> _vertices;
	std::vector<GLuint> _indices;

	GLuint _vao;
	GLuint _vbo;
	GLuint _ibo;
};
