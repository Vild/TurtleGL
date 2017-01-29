#include "mesh.hpp"

#include <cstddef>

Mesh::Mesh(std::shared_ptr<ShaderProgram> program, std::vector<Vertex> vertices, std::vector<GLuint> indices, const std::string& texture)
	: _program(program), _vertices(vertices), _indices(indices) {
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	GLuint buffers[2];
	glGenBuffers(sizeof(buffers) / sizeof(*buffers), buffers);
	_vbo = buffers[0];
	_ibo = buffers[1];
	this->loadObj("D:\\Objects\\hitler\\hitler_full.obj");
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

	_program->bind();

	GLint vertPos = _program->getAttribute("vertPos");
	GLint vertNormal = _program->getAttribute("vertNormal");
	GLint vertColor = _program->getAttribute("vertColor");
	GLint vertUV = _program->getAttribute("vertUV");

	glEnableVertexAttribArray(vertPos);
	// glEnableVertexAttribArray(vertNormal);
	glEnableVertexAttribArray(vertColor);
	glEnableVertexAttribArray(vertUV);

	glVertexAttribPointer(vertPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	// glVertexAttribPointer(vertNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glVertexAttribPointer(vertColor, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glVertexAttribPointer(vertUV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));

	{
		SDL_Surface* surface = IMG_Load(texture.c_str());
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
	glDeleteBuffers(sizeof(buffers) / sizeof(*buffers), buffers);

	glDeleteVertexArrays(1, &_vao);
}

void Mesh::render(const glm::mat4& mvp, const glm::mat4& m) {
	_program->bind();
	glBindVertexArray(_vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	_program->setUniform("mvp", mvp).setUniform("m", m).setUniform("tex", 0).setUniform("diffusePos", glm::vec3(0, 0, -2));
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, NULL);
}

void Mesh::loadObj(std::string fileName) {
	std::vector<unsigned int> vIndicies, uvIndicies, nIndicies;
	std::vector<glm::vec3> tmp_vertices;
	std::vector<glm::vec2> tmp_uvs;
	std::vector<glm::vec3> tmp_normal;
	FILE* file = fopen(fileName.c_str(), "r");
	if (file == nullptr) {
		printf("Unable to load file!\n");
	}
	else {
		while (true) {
			char line[256];	// Not sure how long a line can be in a .obj file.
			int erroCheck = fscanf(file, "%s", line);
			if (erroCheck == EOF)
				break;
			else {
				if (strcmp(line, "v") == 0) {
					glm::vec3 vertex;
					fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
					tmp_vertices.push_back(vertex);
				}
				else if (strcmp(line, "vt") == 0) {
					glm::vec2 uv;
					fscanf(file, "%f %f\n", &uv.x, &uv.y);
					tmp_uvs.push_back(uv);
				}
				else if (strcmp(line, "vn") == 0) {
					glm::vec3 normal;
					fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
					tmp_normal.push_back(normal);
				}
				else if (strcmp(line, "f") == 0) {
					std::string v1, v2, v3;
					unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
					int worked = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
						&vertexIndex[0], &uvIndex[0], &normalIndex[0],
						&vertexIndex[1], &uvIndex[1], &normalIndex[1],
						&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
					if (worked != 9) {
						printf("File can't be read by this pleb loader lmao");
						break;
					}
					// Sends them in 0 -> 2 -> 1 wise because openGL is CW.
					vIndicies.push_back(vertexIndex[0]);
					vIndicies.push_back(vertexIndex[1]);
					vIndicies.push_back(vertexIndex[2]);

					uvIndicies.push_back(uvIndex[0]);
					uvIndicies.push_back(uvIndex[1]);
					uvIndicies.push_back(uvIndex[2]);

					nIndicies.push_back(normalIndex[0]);
					nIndicies.push_back(normalIndex[1]);
					nIndicies.push_back(normalIndex[2]);

				}
			}
		}
	}
	Vertex tmp;
	for (unsigned int i = 0; i < vIndicies.size(); i++) {
		unsigned int vertexIndex = vIndicies[i];
		unsigned int uvIndex = uvIndicies[i];
		unsigned int normalIndex = nIndicies[i];
		glm::vec3 vertex = tmp_vertices[vertexIndex - 1];		// Because obj starts at 1
		glm::vec2 uv = tmp_uvs[uvIndex - 1];
		glm::vec3 normal = tmp_normal[normalIndex - 1];
		tmp.position = vertex;
		tmp.uv = uv;
		tmp.normal = normal;
		_vertices.push_back(tmp);
		_indices.push_back(i);
	}
}