#pragma once

#include "entity.hpp"

#include <assimp/Importer.hpp>

class AssimpEntity : public Entity {
public:
	AssimpEntity(const std::string& filename, const std::string& normalTexture = "assets/textures/errorNormal.png");
	virtual ~AssimpEntity();
	virtual void render(GLenum drawMode = GL_TRIANGLES);

	void setTexture(std::shared_ptr<Texture> texture) {
		_texture = texture;
	}

protected:
	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _normalTexture;

private:
	std::shared_ptr<Mesh> _getModel(const std::string& filename);
	std::shared_ptr<Texture> _getTexture(const std::string& filename);
};
