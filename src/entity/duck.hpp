#pragma once

#include "assimpentity.hpp"
class Duck : public AssimpEntity {
public:
	Duck();
	virtual ~Duck();
	virtual void update(float delta);

private:
	glm::mat4 _model;
};
