// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "mesh.hpp"

#include "../engine.hpp"
#include <cstddef>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices) : _vertices(vertices), _indices(indices), _material(Material()) {
	_makeBuffers();
	_uploadData();
}

Mesh::Mesh(const std::string& file) {
	_makeBuffers();
	_loadObj(file);
	_uploadData();
}

Mesh::~Mesh() {
	for (std::map<std::string, GLuint>::iterator it = _extraBuffers.begin(); it != _extraBuffers.end(); ++it) {
		GLuint id = it->second;
		glDeleteBuffers(1, &id);
	}

	GLuint buffers[2] = {_vbo, _ibo};
	glDeleteBuffers(sizeof(buffers) / sizeof(*buffers), buffers);

	glDeleteVertexArrays(1, &_vao);
}

Mesh& Mesh::addBuffer(const std::string& name, std::function<void(GLuint)> bindHelper, GLenum type) {
	glBindVertexArray(_vao);
	GLuint id;
	glGenBuffers(1, &id);

	_extraBuffers[name] = id;
	bindHelper(id);

	return *this;
}

void Mesh::finalize() {
	// TODO:
}

void Mesh::render(size_t count, GLenum drawMode) {
	glBindVertexArray(_vao);
	glDrawElementsInstanced(drawMode, _indices.size(), GL_UNSIGNED_INT, NULL, count);
}

void Mesh::_makeBuffers() {
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	GLuint buffers[2];
	glGenBuffers(sizeof(buffers) / sizeof(*buffers), buffers);
	_vbo = buffers[0];
	_ibo = buffers[1];
}

void Mesh::_loadObj(const std::string& fileName) {
	std::vector<int> vIndicies, uvIndicies, nIndicies;
	std::vector<glm::vec3> tmp_vertices;
	std::vector<glm::vec2> tmp_uvs;
	std::vector<glm::vec3> tmp_normal;
	char material_Name[80];
	char material_Filename[80];
	bool materials_Loaded = false;
	FILE* file = fopen(fileName.c_str(), "r");
	if (file == nullptr) {
		printf("Unable to load file!: %s\n", fileName.c_str());
	} else {
		while (true) {
			char line[256]; // Not sure how long a line can be in a .obj file.
			int errorCheck = fscanf(file, "%s", line);
			if (errorCheck == EOF)
				break;
			else {
				if (strcmp(line, "mtllib") == 0) {
					fscanf(file, "%s\n", material_Filename);
				} else if (strcmp(line, "v") == 0) {
					glm::vec3 vertex;
					fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
					tmp_vertices.push_back(vertex);
				} else if (strcmp(line, "vt") == 0) {
					glm::vec2 uv;
					fscanf(file, "%f %f\n", &uv.x, &uv.y);
					tmp_uvs.push_back(uv);
				} else if (strcmp(line, "vn") == 0) {
					glm::vec3 normal;
					fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
					tmp_normal.push_back(normal);
				} else if (strcmp(line, "usemtl") == 0 && !materials_Loaded) {
					fscanf(file, "%s\n", material_Name);
					std::string theFilename = std::string("assets/objects/") + material_Filename;
					FILE* mtl_file = fopen(theFilename.c_str(), "r");
					if (mtl_file == nullptr) {
						printf("Unable to load file!: %s\n", theFilename.c_str());
					} else {
						char comparison[80] = {0};
						while (true) {
							// Loads in another material for each usemtl call.
							Material tmp_material;
							errorCheck = fscanf(mtl_file, "%s", line);
							if (errorCheck == EOF) {
								materials_Loaded = true;
								break;
							} else {
								if (strcmp(line, "newmtl") == 0)
									fscanf(mtl_file, "%s\n", comparison);
								if (strcmp(comparison, material_Name) == 0) {
									if (strcmp(line, "Kd") == 0) {
										fscanf(mtl_file, "%f %f %f\n", &_material.kd.x, &_material.kd.y, &_material.kd.z);
									} else if (strcmp(line, "Ka") == 0) {
										fscanf(mtl_file, "%f %f %f\n", &_material.ka.x, &_material.ka.y, &_material.ka.z);
									} else if (strcmp(line, "Tf") == 0) {
										fscanf(mtl_file, "%f %f %f\n", &_material.tf.x, &_material.tf.y, &_material.tf.z);
									} else if (strcmp(line, "Ni") == 0) {
										fscanf(mtl_file, "%f\n", &_material.ni);
									} else if (strcmp(line, "Ks") == 0) {
										fscanf(mtl_file, "%f %f %f\n", &_material.ks.x, &_material.ks.y, &_material.ks.z);
									} else if (strcmp(line, "map_Kd") == 0) {
										char texture_fileName[80];
										fscanf(mtl_file, "%s\n", texture_fileName);
										_material.map_Kd = Engine::getInstance().getTextureManager()->getTexture(texture_fileName); // std::make_shared<Texture>(texture_fileName);
										break;
									}
								}
							}
						}
					}
					materials_Loaded = true;
					fclose(mtl_file);
				} else if (strcmp(line, "f") == 0) {
					unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
					int worked = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1],
															&normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
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
					// printf("%d/%d/%d %d/%d/%d %d/%d/%d\n", vertexIndex[0], uvIndex[0], normalIndex[0], vertexIndex[1], uvIndex[1], normalIndex[1], vertexIndex[2],
					//			 uvIndex[2], normalIndex[2]);
				}
			}
		}
		Vertex tmp;
		for (unsigned int i = 0; i < vIndicies.size(); i++) {
			unsigned int vertexIndex = vIndicies[i];
			unsigned int uvIndex = uvIndicies[i];
			unsigned int normalIndex = nIndicies[i];
			glm::vec3 vertex = tmp_vertices[vertexIndex - 1]; // Because obj starts at 1
			glm::vec2 uv = tmp_uvs[uvIndex - 1];
			glm::vec3 normal = tmp_normal[normalIndex - 1];
			tmp.position = vertex;
			tmp.uv = uv;
			tmp.normal = normal;
			tmp.color = glm::vec3(0, 0, 255);
			_vertices.push_back(tmp);
			_indices.push_back(i);
		}
	}
}

void Mesh::_uploadData() {
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(ShaderAttributeID::vertPos);
	glEnableVertexAttribArray(ShaderAttributeID::vertNormal);
	glEnableVertexAttribArray(ShaderAttributeID::vertColor);
	glEnableVertexAttribArray(ShaderAttributeID::vertUV);
	glEnableVertexAttribArray(ShaderAttributeID::vertTangent);

	glVertexAttribPointer(ShaderAttributeID::vertPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glVertexAttribPointer(ShaderAttributeID::vertNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glVertexAttribPointer(ShaderAttributeID::vertColor, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glVertexAttribPointer(ShaderAttributeID::vertUV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
	glVertexAttribPointer(ShaderAttributeID::vertTangent, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));
}
