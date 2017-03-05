#pragma once
#include "assimpentity.hpp"

#include <vector>

class PlanetSystem : public AssimpEntity {
public:
	PlanetSystem(glm::vec3 center);
	virtual ~PlanetSystem();
	virtual void update(float delta);

private:
	struct Planet {
		glm::vec3 position; // around _center
		glm::mat4 rotation;
		glm::vec3 velocity;
		float distanceFromCenter;
		float radius;
		float mass;
	};

	glm::vec3 _center; // Where to place it in the world

	std::vector<Planet> _planets;
	std::vector<glm::vec3> _oldPos;
	std::vector<glm::mat4> _models; // For storing the resulting matrix that the GPU will use
};
