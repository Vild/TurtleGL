#pragma once

#include <GL/glew.h>
#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "shader.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv;
};

class Mesh {
public:
	Mesh(std::shared_ptr<ShaderProgram> program, std::vector<Vertex> vertices, std::vector<GLuint> indices);
	virtual ~Mesh();

	void render(const glm::mat4& mvp);
	void loadObj(std::string fileName);

	glm::mat4& getTranslation() { return _translation; }
	const glm::mat4& getTranslation() const { return _translation; }
private:
	std::shared_ptr<ShaderProgram> _program;
	std::vector<Vertex> _vertices;
	std::vector<GLuint> _indices;

	GLuint _vao;
	GLuint _vbo;
	GLuint _ibo;

	glm::mat4 _translation; // for the mvp
};
