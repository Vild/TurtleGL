#include "particles.hpp"
#include <math.h>
#include <stdlib.h>
#include "../engine.hpp"

Particles::Particles(int amnt, std::shared_ptr<Mesh> mesh) : Entity(mesh) {
	_drawCount = amnt;
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
	_particle.resize(_drawCount);
	_matrices.resize(_drawCount);
	for (unsigned int i = 0; i < _particle.size(); i++) {
		Particle& p = _particle[i];
		p.spawnLife = rand() % 10 + 1;
		p.spawnPos = glm::vec3(0, 5, 0);
		p.spawnSpeed = glm::normalize(glm::vec3(sin(3 * i), cos(i * 0.5f), cos(i * 0.5f) - sin(8 * i) + sin(3 * i)));

		p.life = p.spawnLife;
		p.speed = p.spawnSpeed;
		p.pos = p.spawnPos;
	}
	_texture = Engine::getInstance().getTextureManager()->getTexture("assets/textures/stevan_god.png");
}

Particles::~Particles() {}

void Particles::render(GLenum drawMode) {
	_texture->bind(0);
	// Should probably have normal texture here.
	Entity::render(drawMode);
}

void Particles::update(float delta) {
	for (unsigned int i = 0; i < _particle.size(); i++) {
		Particle& p = _particle[i];
		if (p.life > 0.0f) {
			p.life -= delta;

			if (p.life > 0.0f) {
				p.speed += p.spawnSpeed * 0.1f;
				p.pos += p.speed * delta;
			}
		} else {
			p.life = p.spawnLife;
			p.speed = p.spawnSpeed;
			p.pos = p.spawnPos;
		}

		p.m = glm::translate(p.pos) * glm::scale(glm::vec3(1.0f));
		_matrices[i] = _particle[i].m;
	}
	_mesh->uploadBufferArray("m", _matrices);
}
