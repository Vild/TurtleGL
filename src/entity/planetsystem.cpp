// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "planetsystem.hpp"

#include "../engine.hpp"
#include <iostream>

static const float G = 6.67384E-11;

PlanetSystem::PlanetSystem(glm::vec3 center) : AssimpEntity("assets/objects/planets/earth.3DS", "assets/objects/planets/4096_NOR.JPG"), _center(center) {
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

	glm::mat3 fix = glm::rotate((float)M_PI / 2.0f, glm::vec3(-1, 0, 0)); // Because the model is rotated by default

	{
		Planet& sun = _planets[0];
		sun.position = glm::vec3(0, 0, 0);
		sun.rotation = fix;
		sun.distanceFromCenter = 0;
		sun.radius = 25.0f;
		sun.mass = 33330.0f;
	}
	{
		Planet& p = _planets[1];
		p.position = glm::vec3(8, 0, 0);
		p.rotation = fix;
		p.distanceFromCenter = 0;
		p.radius = 0.3829f;
		p.mass = 0.553f;
	}
	{
		Planet& p = _planets[2];
		p.position = glm::vec3(12, 0, 0);
		p.rotation = fix;
		p.distanceFromCenter = 0;
		p.radius = 0.9499f;
		p.mass = 0.815f;
	}
	{
		Planet& p = _planets[3];
		p.position = glm::vec3(15, 0, 0);
		p.rotation = fix;
		p.distanceFromCenter = 0;
		p.radius = 1.0f;
		p.mass = 1.0f;
	}
	{
		Planet& p = _planets[4];
		p.position = glm::vec3(20, 0, 0);
		p.rotation = fix;
		p.distanceFromCenter = 0;
		p.radius = 0.5320f;
		p.mass = 0.107f;
	}
	{
		Planet& p = _planets[5];
		p.position = glm::vec3(28, 0, 0);
		p.rotation = fix;
		p.distanceFromCenter = 0;
		p.radius = 10.97f;
		p.mass = 317.83f;
	}
	{
		Planet& p = _planets[6];
		p.position = glm::vec3(35, 0, 0);
		p.rotation = fix;
		p.distanceFromCenter = 0;
		p.radius = 7.140f;
		p.mass = 95.162f;
	}
	{
		Planet& p = _planets[7];
		p.position = glm::vec3(50, 0, 0);
		p.rotation = fix;
		p.distanceFromCenter = 0;
		p.radius = 6.981f;
		p.mass = 14.536f;
	}
	{
		Planet& p = _planets[8];
		p.position = glm::vec3(60, 0, 0);
		p.rotation = fix;
		p.distanceFromCenter = 0;
		p.radius = 8.865f;
		p.mass = 17.147f;
	}

	for (unsigned int i = 1; i < _drawCount; i++) {
		Planet& p = _planets[i];
		glm::vec3 dir = glm::vec3(0);
		for (unsigned int j = 0; j < _drawCount; j++) {
			if (i == j)
				continue;
			const Planet& p2 = _planets[j];
			glm::vec3 toP2 = glm::normalize(p2.position - p.position);
			dir += toP2 * p2.mass * p.mass;
		}

		// dir = glm::vec3(0, 0, 1);

		float r = glm::length(p.position); // glm::distance(p.position, _planets[0].position);
		float v = glm::sqrt((G * p.mass) / r);
		dir = glm::normalize(dir);
		float t = dir.x;
		dir.x = dir.z;
		dir.z = t;
		p.velocity = dir * v*1000.0f;
	}
}

PlanetSystem::~PlanetSystem() {}

void PlanetSystem::update(float delta) {
	for (unsigned int i = 0; i < _drawCount; i++) {
		Planet& p = _planets[i];
		p.rotation *= glm::rotate(delta / (0.1f * p.radius), glm::vec3(0, 0, -1));

		glm::vec3 force(0);
		for (unsigned int j = 0; j < 1; j++) {
			if (i == j)
				continue;
			Planet& p2 = _planets[j];

			float r = glm::distance(p.position, p2.position);
			float forceAmount = (G * p.mass * p2.mass) / (r * r);

			glm::vec3 direction = glm::normalize(p2.position - p.position);

			force += direction * forceAmount;
		}

		if (i) {
			force = glm::normalize(force);
			std::cout << "Planet " << i << " force: (" << force.x << ", " << force.y << ", " << force.z << ")" << std::endl;

			p.position += p.velocity * delta + (force * delta * delta) / 2.0f;
			p.velocity += force * delta;
			std::cout << "Planet " << i << ": (" << p.position.x << ", " << p.position.y << ", " << p.position.z << ")" << std::endl;
		}
		_models[i] = glm::translate(_center) * glm::translate(p.position) * glm::scale(glm::vec3(p.radius / 1500)) * p.rotation;
	}

	std::cout << std::endl;
	_mesh->uploadBufferArray("m", _models);
}
