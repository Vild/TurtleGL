// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "jeep.hpp"

Jeep::Jeep() : AssimpEntity("assets/objects/jeep1.fbx"), _model(glm::mat4(1)) {
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
Jeep::~Jeep() {}

void Jeep::update(float delta) {
	_model *= glm::rotate(delta, glm::vec3(0, -1.5 * 10, 0));
	glm::mat4 model = glm::translate(glm::vec3(4, -4, 4)) * glm::scale(glm::vec3(0.1f)) * _model;
	_mesh->uploadBufferData("m", model);
}
