#pragma once

#include <GL/glew.h>
#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "shader.hpp"
#include "texture.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv;
};

struct Material {
	glm::vec3	kd;
	glm::vec3	ka;
	glm::vec3	tf;
	float		ni;
	glm::vec3	ks;
	std::string	map_kd;
};


class Mesh {
public:
	Mesh(std::shared_ptr<ShaderProgram> program, std::vector<Vertex> vertices, std::vector<GLuint> indices);
	Mesh(std::shared_ptr<ShaderProgram> program, const std::string & file);
	virtual ~Mesh();

	void render(const glm::mat4& mvp);

	glm::mat4& getTranslation() { return _translation; }
	const glm::mat4& getTranslation() const { return _translation; }
private:
	std::shared_ptr<ShaderProgram> _program;
	std::vector<Vertex> _vertices;
	std::vector<GLuint> _indices;

	std::vector<Material>		_materials;
	GLuint _vao;
	GLuint _vbo;
	GLuint _ibo;

	glm::mat4 _translation; // for the mvp

	void _makeBuffers();
	void _loadObj(const std::string& fileName);
	void _uploadData();
};
