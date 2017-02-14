#pragma once

#include <memory>

#include "assimpentity.hpp"

class Box : public AssimpEntity {
public:
	Box(std::shared_ptr<ShaderProgram> program);

	virtual void update(float delta);

private:
	glm::mat4 _baseMatrix;
	std::vector<glm::mat4> _matrices;
};
