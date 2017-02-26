#pragma once

#include <memory>

#include "assimpentity.hpp"

class Box : public AssimpEntity {
public:
	Box();

	virtual void update(float delta);
private:
	glm::mat4 _baseMatrix;
	std::vector<glm::mat4> _matrices;
};
