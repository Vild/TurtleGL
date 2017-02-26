#pragma once

#include "assimpentity.hpp"

class Triangle : public AssimpEntity {
public:
	Triangle();
	virtual ~Triangle();

	virtual void update(float delta);

private:
	glm::mat4 _model;
};
