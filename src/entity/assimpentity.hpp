#pragma once

#include "entity.hpp"

#include <assimp/Importer.hpp>

class AssimpEntity : public Entity {
public:
	AssimpEntity(std::vector<std::shared_ptr<ShaderProgram>> programs, const std::string& filename);
	virtual ~AssimpEntity();

	virtual void render(GLenum drawMode = GL_TRIANGLES);

protected:
	std::shared_ptr<Texture> _texture;

private:
	std::shared_ptr<Mesh> _getModel(std::vector<std::shared_ptr<ShaderProgram>> programs, const std::string& filename);
	std::shared_ptr<Texture> _getTexture(const std::string& filename);
};
