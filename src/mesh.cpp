#include "mesh.hpp"

#include <cstddef>

Mesh::Mesh(std::shared_ptr<ShaderProgram> program, std::vector<Vertex> vertices, std::vector<GLuint> indices) : _program(program), _vertices(vertices), _indices(indices) {

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	
	GLuint buffers[2];
	glGenBuffers(sizeof(buffers)/sizeof(*buffers), buffers);
	_vbo = buffers[0];
	_ibo = buffers[1];
	
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

	_program->bind();
	
	GLint vertPos = _program->getAttribute("vertPos");
	//	GLint vertNormal = _program->getAttribute("vertNormal");
	GLint vertColor = _program->getAttribute("vertColor");
	
	glEnableVertexAttribArray(vertPos);
	//glEnableVertexAttribArray(vertNormal);
	glEnableVertexAttribArray(vertColor);
	
	glVertexAttribPointer(vertPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	//	glVertexAttribPointer(vertNormal, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glVertexAttribPointer(vertColor, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
}

Mesh::~Mesh() {
	GLuint buffers[2] = {_vbo, _ibo};
	glDeleteBuffers(sizeof(buffers)/sizeof(*buffers), buffers);
	
	glDeleteVertexArrays(1, &_vao);
}

void Mesh::render(const glm::mat4 & mvp) {
	_program->bind();
	glBindVertexArray(_vao);
	
	//	_program->setUniform("mvp", mvp);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, NULL);
}
