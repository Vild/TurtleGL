#pragma once

#include <memory>

#include "assimpentity.hpp"

class Duck : public AssimpEntity {
public:
	Duck();

	virtual void update(float delta);

private:
	glm::mat4 _baseMatrix;
	std::vector<glm::mat4> _matrices;
};
