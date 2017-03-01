// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "plane.hpp"

#include "../engine.hpp"

Plane::Plane() : AssimpEntity("assets/objects/plane/plane.fbx", "assets/objects/plane/RockDesert004_NRM_3K.jpg"), _model(glm::mat4(1)) {
	_texture = Engine::getInstance().getTextureManager()->getTexture("assets/objects/plane/RockDesert004_COL_VAR1_3K.jpg");
	_mesh
		->addBuffer("m",
								[](GLuint id) {
									glBindBuffer(GL_ARRAY_BUFFER, id);
									glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // Will only be uploaded once

									for (int i = 0; i < 4; i++) {
										glEnableVertexAttribArray(ShaderAttributeID::m + i);
										glVertexAttribPointer(ShaderAttributeID::m + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4) * i));
										glVertexAttribDivisor(ShaderAttributeID::m + i, 1);
									}

									glBindBuffer(GL_ARRAY_BUFFER, 0);
								})
		.finalize();
}

Plane::~Plane() {}

void Plane::update(float delta) {
	_model = glm::scale(glm::vec3(50.0f, 0.1f, 50.0f));
	glm::mat4 model = glm::translate(glm::vec3(0, -2, 0)) * _model;
	_mesh->uploadBufferData("m", model);
}
