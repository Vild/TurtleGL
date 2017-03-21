#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include "windows.h"

#include "glm/glm.hpp"

#include "MapTools.h"

#include "../gl/mesh.hpp"

#include "../entity/Entity.hpp"

class MQuad : public Entity{//MQuad as in Map-Quad (currently needs testing function and a way to store map textures)
public:
	//parent defines which quad this quad is subordinate to
	//vertexes is main quad data (height), ef editet also changes parent data (and data shared with it's "siblings")
	//normals can be derived from the parent, or "baked" in the quad (also updates parent normals)
	//height defines the array height (second bracket)
	//width defines array width (first bracket)
	//the offsets define the offset from the main parents origin position
	MQuad(MQuad* parent, float*** vertexes, glm::vec3*** normals, int height, int width, int offsetX, int offsetY, glm::mat4 model_matrix);

	std::shared_ptr<Mesh> generateMesh();

	void generateTangents();

	void setChildren(MQuad* children[4]);

	int getWidth();
	int getHeight();

	MQuad* getChild(int pos);

	float*** getVertexes();

	glm::vec3*** getNormals();

	int getOffsetX();

	int getOffsetY() {
		return offsetY;
	}

	glm::vec3* selectNormal(float x, float y, float scale);

	void setRadius();

	void setCenter();

	float getRadius();

	void setVertexValue(int x, int y, float val);

	string printAll();
	string printVec3(glm::vec3 &in);
	string print();

	bool test(glm::vec4* planes);

	void render();

	float calcHeight(glm::vec3 position, float scale);

	glm::mat4 getModelMatrix();

	~MQuad();

private:
	float radius;
	int offsetX;
	int offsetY;
	int height;
	int width;
	glm::vec3 center;
	glm::vec3*** normals;
	float*** vertexes;
	MQuad* parent = nullptr;
	MQuad* children[4] = {nullptr};

	glm::mat4 model_matrix;
};

#endif}