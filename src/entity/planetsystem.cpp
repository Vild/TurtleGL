// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "planetsystem.hpp"

#include "../engine.hpp"
#include <iostream>
#include <fstream>

void from_json(const json& j, Planet& p) {
	p.name = j["name"].get<std::string>();
	p.distanceFromCenter = j["distanceFromCenter"].get<float>();
	p.radius = j["radius"].get<float>();
	p.mass = j["mass"].get<float>();
}

PlanetSystem::PlanetSystem(const std::string& planetsConfig, glm::vec3 center)
	: AssimpEntity("assets/objects/planets/earth.3DS", "assets/objects/planets/4096_NOR.JPG"), _center(center) {
	{
		json planetsJson;
		std::ifstream in(planetsConfig);
		in >> planetsJson;
		for (auto& planet : planetsJson)
			_planets.push_back(planet.get<Planet>());
	}
	_drawCount = _planets.size();
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
	_oldPos.resize(_drawCount);
	_models.resize(_drawCount);

	const glm::mat3 fix = glm::rotate((float)M_PI / 2.0f, glm::vec3(-1, 0, 0)); // Because the model is rotated by default

	for (unsigned int i = 0; i < _drawCount; i++) {
		Planet& p = _planets[i];
		p.rotation = fix;
		p.distanceFromCenter *= ValueScalar;
		p.position = glm::vec3{p.distanceFromCenter, 0, 0};
		p.radius *= ValueScalar;
		p.mass *= ValueScalar;
	}

	for (unsigned int i = 1; i < _drawCount; i++) {
		Planet& p = _planets[i];
		const Planet& sun = _planets[0];
		p.velocity = glm::vec3(0, 0, 0);
		glm::vec3 dir = glm::cross(glm::normalize(p.position - sun.position), glm::vec3{0, 1, 0}); // glm::vec3(0, 0, 1);
		float v = glm::sqrt((G * sun.mass) / p.distanceFromCenter);
		p.velocity += dir * v;
	}
}

PlanetSystem::~PlanetSystem() {}

void PlanetSystem::update(float realDelta) {
	while (realDelta > 0) {
		float delta = glm::min(2.0f, realDelta);
		realDelta -= delta;

		for (unsigned int i = 0; i < _drawCount; i++)
			_oldPos[i] = _planets[i].position;

		for (unsigned int i = 0; i < _drawCount; i++) {
			Planet& p = _planets[i];
			p.rotation *= glm::rotate(delta / (p.radius), glm::vec3(0, 0, -1));

			glm::vec3 force(0);
			for (unsigned int j = 0; j < _drawCount; j++) {
				if (i == j)
					continue;
				float p2Mass = _planets[j].mass;
				glm::vec3 p2Pos = _oldPos[j];

				float r = glm::distance(p.position, p2Pos);
				float forceAmount = (G * p.mass * p2Mass) / (r * r);

				glm::vec3 direction = glm::normalize(p2Pos - p.position);

				force += direction * forceAmount;
			}

			force /= p.mass;
			p.position += p.velocity * delta + (force * delta * delta) / 2.0f;
			p.velocity += force * delta;

			_models[i] = glm::translate(_center) * glm::translate(p.position * DistanceScalar) * glm::scale(glm::vec3(p.radius * ScaleScalar)) * p.rotation;
		}
	}
	_mesh->uploadBufferArray("m", _models);
}
