#pragma once

#include "assimpentity.hpp"
class Jeep : public AssimpEntity {
public:
	Jeep();
	virtual ~Jeep();
	virtual void update(float delta);

private:
	glm::mat4 _model;
};
