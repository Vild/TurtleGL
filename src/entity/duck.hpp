#pragma once

#include "assimpentity.hpp"
class Duck : public AssimpEntity {
public:
	Duck(std::vector<std::shared_ptr<ShaderProgram>> programs);
	virtual ~Duck();
	virtual void update(float delta);

private:
	glm::mat4 _model;
};
