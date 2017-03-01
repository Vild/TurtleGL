// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "duck.hpp"

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <cstdint>

Duck::Duck() : AssimpEntity("assets/objects/duck.fbx"), _baseMatrix(glm::scale(glm::vec3(0.01f))) {
	_drawCount = 27;
	_mesh
		->addBuffer("m",
								[&](GLuint id) {
									glBindBuffer(GL_ARRAY_BUFFER, id);
									glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * _drawCount, NULL, GL_STATIC_DRAW); // Will only be uploaded once

									for (int i = 0; i < 4; i++) {
										glEnableVertexAttribArray(ShaderAttributeID::m + i);
										glVertexAttribPointer(ShaderAttributeID::m + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4) * i));
										glVertexAttribDivisor(ShaderAttributeID::m + i, 1);
									}

									glBindBuffer(GL_ARRAY_BUFFER, 0);
								})
		.finalize();
	_matrices.resize(_drawCount);
}

void Duck::update(float delta) {
	static float timeCounter = 0;
	timeCounter += delta;
	_baseMatrix *= glm::rotate(delta, glm::vec3(0, -1.5 * 10, 0));

	glm::mat4 movement = glm::mat4(1); // glm::translate(glm::vec3(sin(timeCounter) * 4, 0, sin(timeCounter) * 4 - cos(timeCounter) * 4));
	for (int y = 0; y < 3; y++)
		for (int i = 0; i < 9; i++)
			_matrices[y * 9 + i] = (movement * glm::translate(glm::vec3{(i % 3) * 4, y * 4, (i / 3) * 4})) * _baseMatrix;
	_mesh->uploadBufferArray("m", _matrices);
}
