#pragma once

#include <GL/glew.h>
#include <vector>
#include <memory>

#include "texture.hpp"

struct attachment {
	int id;
	std::shared_ptr<Texture> texture;
};

class Framebuffer {
public:
	Framebuffer();
	Framebuffer(GLuint fb);
	virtual ~Framebuffer();

	Framebuffer& bind(bool read = true, bool draw = true);

	Framebuffer& attachTexture(int id, size_t width, size_t height, GLenum dataFormat, GLenum dataType, int vectorSize);
	Framebuffer& attachRenderBuffer(size_t width, size_t height, GLenum format = GL_DEPTH24_STENCIL8);

	Framebuffer& finalize();

	const std::vector<attachment>& getAttachments();

private:
	GLuint _fb;
	std::vector<attachment> _attachments;
	GLuint _renderBuffer;
};
