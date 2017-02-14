#include "sphere.hpp"

#include "../engine.hpp"

Sphere::Sphere(std::shared_ptr<ShaderProgram> program)
	: Entity(std::make_shared<Mesh>(program, "assets/objects/sphere_blue_blinn_760_tris_TRIANGULATED.obj")), _model(glm::mat4(1)) {
	_mesh
		->addBuffer("m",
								[](std::shared_ptr<ShaderProgram> program, GLuint id) {
									GLint m = program->getAttribute("m");
									if (m == -1)
										return;
									glm::mat4 mData = glm::scale(glm::vec3(0.01f));

									glBindBuffer(GL_ARRAY_BUFFER, id);
									glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(mData), GL_STATIC_DRAW); // Will only be uploaded once

									for (int i = 0; i < 4; i++) {
										glEnableVertexAttribArray(m + i);
										glVertexAttribPointer(m + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4) * i));
										glVertexAttribDivisor(m + i, 1);
									}

									glBindBuffer(GL_ARRAY_BUFFER, 0);
								})
		.finalize();
}

Sphere::~Sphere() {}

void Sphere::update(float delta) {
	_model *= glm::rotate(delta, glm::vec3(0, -1.5 * 10, 0));

	glm::mat4 model = _model * glm::translate(glm::vec3(4, 16, 4)) * glm::scale(glm::vec3(2.0f));
	_mesh->uploadBufferData("m", model);
}

void Sphere::render(GLenum drawMode) {
	Engine::getInstance().getTextureManager()->getErrorTexture()->bind(0);
	Entity::render(drawMode);
}
