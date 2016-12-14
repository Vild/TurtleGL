#include "mesh.hpp"

#include <cstddef>


Mesh::Mesh(std::shared_ptr<ShaderProgram> program, std::vector<Vertex> vertices, std::vector<GLuint> indices, const std::string & texture) : _program(program), _vertices(vertices), _indices(indices) {

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	
	GLuint buffers[2];
	glGenBuffers(sizeof(buffers) / sizeof(*buffers), buffers);
	_vbo = buffers[0];
	_ibo = buffers[1];
	
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

	_program->bind();
	
	GLint vertPos = _program->getAttribute("vertPos");
	//GLint vertNormal = _program->getAttribute("vertNormal");
	GLint vertColor = _program->getAttribute("vertColor");
	GLint vertUV = _program->getAttribute("vertUV");
	
	glEnableVertexAttribArray(vertPos);
	//glEnableVertexAttribArray(vertNormal);
	glEnableVertexAttribArray(vertColor);
	glEnableVertexAttribArray(vertUV);
	
	glVertexAttribPointer(vertPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	//glVertexAttribPointer(vertNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glVertexAttribPointer(vertColor, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glVertexAttribPointer(vertUV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));


	{
		SDL_Surface * surface = IMG_Load(texture.c_str());
		if (!surface)
			throw "Texture failed to load!";

		GLenum format;
		
		int nOfColors = surface->format->BytesPerPixel;
		if (nOfColors == 4) {
			if (surface->format->Rmask == 0x000000ff)
        format = GL_RGBA;
			else
        format = GL_BGRA;
		} else if (nOfColors == 3) {
			if (surface->format->Rmask == 0x000000ff)
        format = GL_RGB;
			else
        format = GL_BGR;
		} else
			throw "Invalid texture format";
	 
		glGenTextures(1, &_texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture);
		
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		
		glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

		SDL_FreeSurface(surface);
	}
}

Mesh::~Mesh() {
	glDeleteTextures(1, &_texture);
	
	GLuint buffers[2] = {_vbo, _ibo};
	glDeleteBuffers(sizeof(buffers)/sizeof(*buffers), buffers);
	
	glDeleteVertexArrays(1, &_vao);
}

void Mesh::render(const glm::mat4 & mvp, const glm::mat4 & m) {
	_program->bind();
	glBindVertexArray(_vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	
	_program->setUniform("mvp", mvp).setUniform("m", m).setUniform("tex", 0).setUniform("diffusePos", glm::vec3(0, 0, -2));
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, NULL);
}
