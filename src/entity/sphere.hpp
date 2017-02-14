#pragma once

#include <memory>

class Entity;
#include "entity.hpp"
#include "../gl/shader.hpp"

class Sphere : public Entity {
public:
	Sphere(std::shared_ptr<ShaderProgram> program);
	virtual ~Sphere();

	virtual void update(float delta);
	virtual void render(GLenum drawMode = GL_TRIANGLES);

private:
	glm::mat4 _model;
};
