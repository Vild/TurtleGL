#include "box.hpp"

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <cstdint>

// TODO: Remove this hack when .obj loading is implemented
static std::vector<glm::vec3> verts = {
	glm::vec3{-1.0f, -1.0f, -1.0f}, glm::vec3{-1.0f, -1.0f, 1.0f},	glm::vec3{-1.0f, 1.0f, 1.0f},		glm::vec3{1.0f, 1.0f, -1.0f},	glm::vec3{-1.0f, -1.0f, -1.0f},
	glm::vec3{-1.0f, 1.0f, -1.0f},	glm::vec3{1.0f, -1.0f, 1.0f},		glm::vec3{-1.0f, -1.0f, -1.0f}, glm::vec3{1.0f, -1.0f, -1.0f}, glm::vec3{1.0f, 1.0f, -1.0f},
	glm::vec3{1.0f, -1.0f, -1.0f},	glm::vec3{-1.0f, -1.0f, -1.0f}, glm::vec3{-1.0f, -1.0f, -1.0f}, glm::vec3{-1.0f, 1.0f, 1.0f},	glm::vec3{-1.0f, 1.0f, -1.0f},
	glm::vec3{1.0f, -1.0f, 1.0f},		glm::vec3{-1.0f, -1.0f, 1.0f},	glm::vec3{-1.0f, -1.0f, -1.0f}, glm::vec3{-1.0f, 1.0f, 1.0f},	glm::vec3{-1.0f, -1.0f, 1.0f},
	glm::vec3{1.0f, -1.0f, 1.0f},		glm::vec3{1.0f, 1.0f, 1.0f},		glm::vec3{1.0f, -1.0f, -1.0f},	glm::vec3{1.0f, 1.0f, -1.0f},	glm::vec3{1.0f, -1.0f, -1.0f},
	glm::vec3{1.0f, 1.0f, 1.0f},		glm::vec3{1.0f, -1.0f, 1.0f},		glm::vec3{1.0f, 1.0f, 1.0f},		glm::vec3{1.0f, 1.0f, -1.0f},	glm::vec3{-1.0f, 1.0f, -1.0f},
	glm::vec3{1.0f, 1.0f, 1.0f},		glm::vec3{-1.0f, 1.0f, -1.0f},	glm::vec3{-1.0f, 1.0f, 1.0f},		glm::vec3{1.0f, 1.0f, 1.0f},	 glm::vec3{-1.0f, 1.0f, 1.0f},
	glm::vec3{1.0f, -1.0f, 1.0}};

static std::vector<Vertex> getVertices() {
	std::vector<Vertex> l;
	for (size_t i = 0; i < verts.size(); i += 3) {
		glm::vec3 e0 = verts[i + 1] - verts[i + 0];
		glm::vec3 e1 = verts[i + 2] - verts[i + 0];

		glm::vec3 n = cross(e0, e1);

		l.push_back(Vertex{verts[i], n, {1.0, 1.0, 1.0}, {0, 0}});
		l.push_back(Vertex{verts[i + 1], n, {1.0, 1.0, 1.0}, {0, 1}});
		l.push_back(Vertex{verts[i + 2], n, {1.0, 1.0, 1.0}, {1, 0}});
	}

	return l;
}
static std::vector<GLuint> getIndicies() {
	std::vector<GLuint> l;
	for (size_t i = 0; i < verts.size(); i += 3) {
		l.push_back(i + 0);
		l.push_back(i + 1);
		l.push_back(i + 2);
	}
	return l;
}

Box::Box(std::shared_ptr<ShaderProgram> program) : Mesh(program, getVertices(), getIndicies()) {}
