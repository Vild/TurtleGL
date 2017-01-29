#pragma once

#include <memory>

#include "../mesh.hpp"
#include "../shader.hpp"

class Box : public Mesh {
public:
	Box(std::shared_ptr<ShaderProgram> program);
};
