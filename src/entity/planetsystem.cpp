// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "planetsystem.hpp"

#include "../engine.hpp"

PlanetSystem::PlanetSystem() : AssimpEntity("assets/objects/planets/earth.3DS", "assets/objects/planets/4096_NOR.JPG") {
	_drawCount = 9;
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
	_planets.resize(_drawCount);
	_models.resize(_drawCount);
}

PlanetSystem::~PlanetSystem() {}

void PlanetSystem::update(float delta) {
	for (unsigned int i = 0; i < _drawCount; i++) {
		Planet& planet = _planets[i];


		//TODO: Implement real physics here

		planet.model *= glm::rotate(delta * (i/2), glm::vec3(0, -1, 0));

		glm::mat4 model = planet.model * glm::translate(glm::vec3(4 + i * 4, 4, 4 + i * 4)) * glm::scale(glm::vec3(1.0f / 150)) *
											glm::rotate((float)M_PI / 2.0f, glm::vec3(-1, 0, 0));

		_models[i] = model;
	}

	_mesh->uploadBufferArray("m", _models);
}
