#pragma once
#include "assimpentity.hpp"

#include <vector>
#include "../lib/json.hpp"

using json = nlohmann::json;
struct Planet {
	std::string name;
	glm::vec3 position; // around _center
	glm::mat4 rotation;
	glm::vec3 velocity;
	float distanceFromCenter;
	float radius;
	float mass;
};

void from_json(const json& j, Planet& p);

class PlanetSystem : public AssimpEntity {
public:
	PlanetSystem(const std::string& planetsConfig, glm::vec3 center);
	virtual ~PlanetSystem();
	virtual void update(float delta);

private:
	static constexpr float G = 6.67384E-11;
	static constexpr float ValueScalar = 1E0;
	static constexpr float DistanceScalar = 1E0;
	static constexpr float ScaleScalar = 1E-3;

	glm::vec3 _center; // Where to place it in the world

	std::vector<Planet> _planets;
	std::vector<glm::vec3> _oldPos;
	std::vector<glm::mat4> _models; // For storing the resulting matrix that the GPU will use
};
