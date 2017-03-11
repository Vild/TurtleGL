#pragma once
#include "entity.hpp"

class Particles : public Entity {
public:
	Particles(int amnt, std::shared_ptr<Mesh> mesh);
	virtual ~Particles();
	virtual void update(float delta);
private:
	struct Particle {
		glm::vec3 spawnPos, spawnSpeed, pos, speed;
		unsigned char r, g, b, a; // Color
		float size, angle, weight;
		float spawnLife, life;
		glm::mat4 m;
	};
	glm::mat4 _model;
	std::vector<Particle> _particle;
	std::vector<glm::mat4> _matrices;
};