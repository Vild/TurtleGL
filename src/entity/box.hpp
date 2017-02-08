#pragma once

#include <memory>

#include "../gl/mesh.hpp"
#include "../gl/shader.hpp"

class Box : public Mesh {
public:
	Box(std::shared_ptr<ShaderProgram> program);
};
