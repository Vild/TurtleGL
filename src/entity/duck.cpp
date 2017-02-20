#include "duck.hpp"

Duck::Duck(std::vector<std::shared_ptr<ShaderProgram>> programs) : AssimpEntity(programs, "assets/objects/jeep1.fbx"), _model(glm::mat4(1)) {
	_mesh
		->addBuffer("m",
								[](std::vector<std::shared_ptr<ShaderProgram>> programs, GLuint id) {
									glBindBuffer(GL_ARRAY_BUFFER, id);
									glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // Will only be uploaded once

									for (auto program : programs) {
										program->bind();
										GLint m = program->getAttribute("m");
										if (m == -1)
											return;
										for (int i = 0; i < 4; i++) {
											glEnableVertexAttribArray(m + i);
											glVertexAttribPointer(m + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4) * i));
											glVertexAttribDivisor(m + i, 1);
										}
									}

									glBindBuffer(GL_ARRAY_BUFFER, 0);
								})
		.finalize();
}
Duck::~Duck() {}

void Duck::update(float delta) {
	_model *= glm::rotate(delta, glm::vec3(0, -1.5 * 10, 0));

	glm::mat4 model = glm::translate(glm::vec3(4, -4, 4)) * glm::scale(glm::vec3(0.1f)) * _model;
	_mesh->uploadBufferData("m", model);
}
