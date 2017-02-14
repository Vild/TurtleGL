#pragma once

#include "assimpentity.hpp"
class Duck : public AssimpEntity {
public:
	Duck(std::shared_ptr<ShaderProgram> program);
	virtual ~Duck();
	virtual void update(float delta);

private:
	glm::mat4 _model;
};
