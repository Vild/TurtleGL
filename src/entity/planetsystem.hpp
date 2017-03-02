#pragma once
#include "assimpentity.hpp"

#include <vector>

class PlanetSystem : public AssimpEntity {
public:
	PlanetSystem();
	virtual ~PlanetSystem();
	virtual void update(float delta);

private:
	struct Planet {
		glm::mat4 selfRotation;
		glm::mat4 model; //Position and rotation around _center
		float size;
	};

	glm::vec3 _center;

	std::vector<Planet> _planets;
	std::vector<glm::mat4> _models;
};
