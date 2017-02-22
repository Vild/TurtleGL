#pragma once

#include <memory>

#include "assimpentity.hpp"

class Earth : public AssimpEntity {
public:
	Earth(std::vector<std::shared_ptr<ShaderProgram>> programs);
	virtual ~Earth();

	virtual void update(float delta);

private:
	glm::mat4 _model;
};
