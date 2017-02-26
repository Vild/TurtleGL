#pragma once

#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdint>

#include "../gl/mesh.hpp"

class Entity {
public:
	Entity(std::shared_ptr<Mesh> mesh);
	virtual ~Entity();

	virtual void update(float delta);
	virtual void render(GLenum drawMode = GL_TRIANGLES);
protected:
	unsigned int _drawCount = 1;
	std::shared_ptr<Mesh> _mesh;
};
