#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>

#include "glm/glm.hpp"

#include "../gl/mesh.hpp"

#include "entity.hpp"

class MapQuad : public Entity {
public:
	MapQuad(MapQuad* parent, float*** vertexes, glm::vec3*** normals, int height, int width, int offsetX, int offsetY);

	std::shared_ptr<Mesh> generateMesh(int width, int height, float*** vertexes, glm::vec3*** normals, int offsetX, int offsetY);
	Vertex createVertex(glm::vec3 pos, glm::vec3 color, glm::vec3 tangent, glm::vec3 normal, glm::vec2 uv);
	void setRadius();
	void setCenter();
	void setChildren(MapQuad* children[4]);

	int getWidth();
	int getHeight();

	MapQuad* getChild(int pos);

	float*** getVertexes();
	glm::vec3*** getNormals();

	int getOffsetX();
	int getOffsetY();
	glm::vec3 getCenter();

	void cull();

	glm::vec3* selectNormal(float x, float y, float scale);

	float getRadius();

	void setVertexValue(int x, int y, float val);

	std::string printAll();
	std::string printVec3(glm::vec3& in);
	std::string print();

	bool test(glm::vec4* planes, glm::mat4 vp);

	void update(float delta);

	virtual void render(GLenum drawMode = GL_TRIANGLES);

	float calcHeightOfMesh(glm::vec3 position);

	glm::vec3 pointOnMesh(glm::vec3 position);

	bool isOutsideQuad(glm::vec3 position);

	virtual ~MapQuad();

protected:
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Texture> normalTexture;

private:
	float radius;
	int offsetX;
	int offsetY;
	int height;
	int width;
	glm::vec3 center;
	glm::vec3*** normals;
	float*** vertexes;
	MapQuad* parent = nullptr;
	MapQuad* children[4] = {nullptr};

	glm::mat4 model_matrix;
	bool renderState;
};
