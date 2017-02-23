#pragma once

#include <memory>

#include "assimpentity.hpp"

class Earth : public AssimpEntity {
public:
	Earth();
	virtual ~Earth();

	virtual void update(float delta);

private:
	glm::mat4 _model;
};
