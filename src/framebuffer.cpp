#include "framebuffer.hpp"

Framebuffer::Framebuffer() {
	glGenFramebuffers(1, &_fb);
	_renderBuffer = 0;
}

Framebuffer::Framebuffer(GLuint fb) : _fb(fb), _renderBuffer(0) {}

Framebuffer::~Framebuffer() {
	if (_renderBuffer)
		glDeleteRenderbuffers(1, &_renderBuffer);

	_attachments.clear();

	// I use a framebuffer object with the id zero for the screen.
	// So don't try and remove that one. OpenGL will (probably) be mad if you do that.
	if (_fb)
		glDeleteFramebuffers(1, &_fb);
}

Framebuffer& Framebuffer::bind(bool read, bool draw) {
	if (read) {
		if (draw)
			glBindFramebuffer(GL_FRAMEBUFFER, _fb);
		else
			glBindFramebuffer(GL_READ_FRAMEBUFFER, _fb);
	} else
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fb);

	return *this;
}

Framebuffer& Framebuffer::attachTexture(int id, size_t width, size_t height, GLenum dataFormat, GLenum dataType, int vectorSize) {
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	GLenum formats[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};

	glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, width, height, 0, formats[vectorSize - 1], dataType, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + id, texID, 0);

	_attachments.push_back(attachment{id, std::make_shared<Texture>(texID)});

	return *this;
}

Framebuffer& Framebuffer::attachRenderBuffer(size_t width, size_t height, GLenum format) {
	glGenRenderbuffers(1, &_renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderBuffer);
	return *this;
}

Framebuffer& Framebuffer::finalize() {
	std::vector<GLenum> buffers;
	for (size_t i = 0; i < _attachments.size(); i++)
		buffers.push_back(GL_COLOR_ATTACHMENT0 + _attachments[i].id);
	glDrawBuffers(buffers.size(), &buffers[0]);
	return *this;
}

const std::vector<attachment>& Framebuffer::getAttachments() {
	return _attachments;
}
