// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "entity.hpp"

Entity::Entity(std::shared_ptr<Mesh> mesh) : _mesh(mesh) {}
Entity::~Entity() {}

void Entity::update(float delta) {}

void Entity::render(GLenum drawMode) {
	_mesh->render(_drawCount, drawMode);
}
