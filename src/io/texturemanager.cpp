#include "texturemanager.hpp"

#include <iostream>
#include <exception>

TextureManager::TextureManager() : _errorTexture(_loadErrorTexture()) {}

TextureManager::~TextureManager() {
	_storage.clear();
}

std::shared_ptr<Texture> TextureManager::getTexture(const std::string& file) {
	std::cout << "Want file: " << file << std::endl;
	try {
		std::shared_ptr<Texture> tex = std::make_shared<Texture>(file);
		return _storage[file] = tex;
	} catch (const char*) {
	}
	return _storage[file] = _errorTexture;
}

std::shared_ptr<Texture> TextureManager::getErrorTexture() {
	return _errorTexture;
}

std::shared_ptr<Texture> TextureManager::_loadErrorTexture() {
	return std::make_shared<Texture>("assets/textures/error.png");
}
