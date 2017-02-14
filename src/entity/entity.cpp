#include "entity.hpp"

Entity::Entity(std::shared_ptr<Mesh> mesh) : _mesh(mesh) {}
Entity::~Entity() {}

void Entity::update(float delta) {}

void Entity::render(GLenum drawMode) {
	_mesh->render(_drawCount, drawMode);
}
