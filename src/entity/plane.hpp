#pragma once
#include "assimpentity.hpp"

class Plane : public AssimpEntity {
public:
	Plane();
	virtual ~Plane();
	virtual void update(float delta);

private:
	glm::mat4 _model;
};
