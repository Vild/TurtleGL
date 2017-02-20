#pragma once

#include <GL/glew.h>
#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <functional>
#include "texture.hpp"

#include "shader.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv;
	glm::vec3 tangent;
};

struct Material {
	glm::vec3 kd;
	glm::vec3 ka;
	glm::vec3 tf;
	float ni;
	glm::vec3 ks;
	std::shared_ptr<Texture> map_Kd;
};

class Mesh {
public:
	Mesh(std::vector<std::shared_ptr<ShaderProgram>> programs, std::vector<Vertex> vertices, std::vector<GLuint> indices);
	Mesh(std::vector<std::shared_ptr<ShaderProgram>> programs, const std::string& file);
	virtual ~Mesh();

	Mesh& addBuffer(const std::string& name, std::function<void(std::vector<std::shared_ptr<ShaderProgram>>, GLuint)> bindHelper, GLenum type = GL_ARRAY_BUFFER);

	void finalize();

	void render(size_t count = 1, GLenum drawMode = GL_TRIANGLES);
	Material& getMaterial() { return _material; };

	template <typename T>
	Mesh& uploadBufferData(const std::string& name, const T& data) {
		try {
			GLuint id = _extraBuffers[name];
			glBindBuffer(GL_ARRAY_BUFFER, id);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(T), glm::value_ptr(data));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		} catch (std::exception e) {
		}
		return *this;
	}

	template <typename T>
	Mesh& uploadBufferArray(const std::string& name, const std::vector<T>& data) {
		try {
			GLuint id = _extraBuffers[name];
			glBindBuffer(GL_ARRAY_BUFFER, id);
			glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(T), &data[0]);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		} catch (std::exception e) {
		}
		return *this;
	}

private:
	std::vector<std::shared_ptr<ShaderProgram>> _programs;
	std::vector<Vertex> _vertices;
	std::vector<GLuint> _indices;
	Material _material;

	GLuint _vao;
	GLuint _vbo;
	GLuint _ibo;

	std::map<std::string, GLuint> _extraBuffers;

	static const int MAX_INSTANCE = 16;

	void _makeBuffers();
	void _loadObj(const std::string& fileName);
	void _uploadData();
};
