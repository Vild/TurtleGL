#include "MapQuad.hpp"

#include "../engine.hpp"

MapQuad::MapQuad(MapQuad* parent, float*** vertexes, glm::vec3*** normals, int height, int width, int offsetX, int offsetY) :
	Entity(generateMesh(width, height, vertexes, normals, offsetX, offsetY)), texture(Engine::getInstance().getTextureManager()->getTexture("assets/textures/ground1.png")), normalTexture(Engine::getInstance().getTextureManager()->getTexture("assets/textures/errorNormal.png"))
{
	this->width = width;
	this->height = height;
	this->parent = parent;
	this->vertexes = vertexes;
	this->normals = normals;
	this->offsetX = offsetX;
	this->offsetY = offsetY;

	setCenter();
	setRadius();

	this->model_matrix = model_matrix;
	renderState = false;

	_mesh
		->addBuffer("m",
			[](GLuint id) {
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // Will only be uploaded once

		for (int i = 0; i < 4; i++) {
			glEnableVertexAttribArray(ShaderAttributeID::m + i);
			glVertexAttribPointer(ShaderAttributeID::m + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(ShaderAttributeID::m + i, 1);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	})
		.finalize();

	glm::mat3 t = { 1.f,2.f,3.f,
					4.f,5.f,6.f,
					7.f,8.f,9.f };
	t = glm::transpose(t);
}

std::shared_ptr<Mesh> MapQuad::generateMesh(int width, int height, float*** vertexes, glm::vec3*** normals, int offsetX, int offsetY) {
	std::vector<Vertex> vertexVector;
	std::vector<GLuint> indVector;
	int ind=0;
	for (int x = 0; x < width - 1; x++) {
		for (int y = 0; y < height - 1; y++) {
			glm::vec3 pos, color, tangent, normal;
			glm::vec2 uv;

			color = glm::vec3(1.f, 1.f, 1.f);
			tangent = glm::vec3(1.f, 1.f, 1.f);

			//lower left

			pos = glm::vec3((float)x + offsetX, *vertexes[x][y], (float)y + offsetY);
			normal = *normals[x*2][y];
			uv = glm::vec2(0, 0);
			vertexVector.push_back(createVertex(pos, color, tangent, -normal, uv));

			pos = glm::vec3((float)x + offsetX, *vertexes[x][y + 1], (float)y + 1 + offsetY);
			normal = *normals[x * 2][y];
			uv = glm::vec2(0, 1);
			vertexVector.push_back(createVertex(pos, color, tangent, -normal, uv));

			pos = glm::vec3((float)x + 1 + offsetX, *vertexes[x + 1][y + 1], (float)y + 1 + offsetY);
			normal = *normals[x * 2][y];
			uv = glm::vec2(1, 1);
			vertexVector.push_back(createVertex(pos, color, tangent, -normal, uv));

			//upper right

			pos = glm::vec3((float)x + offsetX, *vertexes[x][y], (float)y + offsetY);
			normal = *normals[(x * 2) + 1][y];
			uv = glm::vec2(0, 0);
			vertexVector.push_back(createVertex(pos, color, tangent, normal, uv));

			pos = glm::vec3((float)x + 1 + offsetX, *vertexes[x + 1][y + 1], (float)y + 1 + offsetY);
			normal = *normals[(x * 2) + 1][y];
			uv = glm::vec2(1, 1);
			vertexVector.push_back(createVertex(pos, color, tangent, normal, uv));

			pos = glm::vec3((float)x + 1 + offsetX, *vertexes[x + 1][y], (float)y + offsetY);
			normal = *normals[(x * 2) + 1][y];
			uv = glm::vec2(1, 0);
			vertexVector.push_back(createVertex(pos, color, tangent, normal, uv));

			indVector.push_back(ind);
			ind++;
			indVector.push_back(ind);
			ind++;
			indVector.push_back(ind);
			ind++;
			indVector.push_back(ind);
			ind++;
			indVector.push_back(ind);
			ind++;
			indVector.push_back(ind);
			ind++;
		}
	}
	return std::make_shared<Mesh>(vertexVector, indVector);
}

Vertex MapQuad::createVertex(glm::vec3 pos, glm::vec3 color, glm::vec3 tangent, glm::vec3 normal, glm::vec2 uv) {
	Vertex vertex;
	vertex.position = pos;
	vertex.color = color;
	vertex.normal = normal;
	vertex.tangent = tangent;
	vertex.uv = uv;
	return vertex;
}

void MapQuad::setRadius() {//Used for the culling process, if sphere generated by the center and the radius intersects with the frustrum the quad will be rendered
	radius = 0.0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			glm::vec3 point((float)x, (float)y, *vertexes[x][y]);
			float length = glm::length(point - center);
			if (length>radius) {
				radius = std::abs(length);
			}
		}
	}
}

void MapQuad::setCenter() {//defines the quads center from its average height and offset position
	float avgHeight = 0.0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			avgHeight += *vertexes[x][y];
		}
	}
	avgHeight /= (float)(height*width);

	center = glm::vec3(((float)width / 2.f) + offsetX, avgHeight, ((float)height / 2.f) + offsetY);
}

void MapQuad::setChildren(MapQuad* children[4]) {//assign new quads as children to this quad
	for (int i = 0; i < 4; i++) {
		this->children[i] = children[i];
	}
}

int MapQuad::getWidth() {
	return width;
}
int MapQuad::getHeight() {
	return height;
}

MapQuad* MapQuad::getChild(int pos) {//return child, 0-3 as values
	return children[pos];
}

float*** MapQuad::getVertexes() {//return all vertex values (for rendering)
	return vertexes;
}

glm::vec3*** MapQuad::getNormals() {//return all normals (also for rendering)
	return normals;
}

int MapQuad::getOffsetX() {
	return offsetX;
}

int MapQuad::getOffsetY() {
	return offsetY;
}

glm::vec3* MapQuad::selectNormal(float x, float y, float scale) {//returns the pre-computed normal for a vertex at given position

	float xs = x / scale;//scale to localspace
	float ys = y / scale;

	if (isOutsideQuad(glm::vec3(x,0,y))) {
		return new glm::vec3(0.0f, 1.0f, 0.0f);
	}

	int Xaprox = nearbyintf(xs);//round to find nearest int (vertex)
	int XaproxPartner;
	if (Xaprox - xs < 0) {//xs larger than Xaprox, xs has been rounded down (result negative)
		XaproxPartner = Xaprox + 1;
	}
	else {//xs smaller than Xaprox, xs has been rounded up (result positive)
		Xaprox--;
		XaproxPartner = Xaprox + 1;
	}

	int Yaprox = nearbyintf(ys);//round to find nearest int (vertex)
	int YaproxPartner;
	if (Yaprox - ys < 0) {//ys larger than Yaprox, ys has been rounded down (result negative)
		YaproxPartner = Yaprox + 1;
	}
	else {//ys smaller than Yaprox, ys has been rounded up (result positive)
		Yaprox--;
		YaproxPartner = Yaprox + 1;
	}

	glm::vec2 downLeft(Xaprox, YaproxPartner);
	glm::vec2 upRight(XaproxPartner, Yaprox);
	glm::vec2 p(xs, ys);
	glm::vec2 d1 = p - downLeft;
	glm::vec2 d2 = p - upRight;

	if (d1.length() < d2.length()) {//d1 is dominant, the normal we seek is in the triangle in the bottom left of the quad
		return normals[Xaprox * 2][YaproxPartner];
	}
	else {//else its the top right one
		return normals[(Xaprox * 2) + 1][Yaprox];
	}
}

float MapQuad::getRadius() {
	return radius;
}

void MapQuad::setVertexValue(int x, int y, float val) {//can manipulate the height of a vertex, a call to MapTools::calculateNormals() should be performed afterwards
	*vertexes[x][y] = val;
}

std::string MapQuad::printAll() {//returns the information for THIS QUAD and ALL under it (debug)
	std::stringstream OUTPUT;
	OUTPUT << print();
	for (int i = 0; i < 4; i++) {
		if (children[i] != nullptr) {
			OUTPUT << children[i]->printAll();
		}
	}
	return OUTPUT.str();
}

std::string MapQuad::printVec3(glm::vec3 &in) {//prints a glm::vec3 in a nice way (debug)
	std::stringstream OUTPUT;
	OUTPUT << "(" << in.x << ", " << in.y << ", " << in.z << ")";
	return OUTPUT.str();
}

std::string MapQuad::print() {//main quad printer (debug)
	std::stringstream OUTPUT;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			OUTPUT << std::setw(3) << std::setfill('0') << *vertexes[x][y] << " ";
		}
		if (y == 0) {
			OUTPUT << "	Width: " << width << " Offset: " << offsetX;
		}
		if (y == 1) {
			OUTPUT << "	Height: " << height << " Offset: " << offsetY;
		}
		if (y == 2) {
			OUTPUT << "	Radius: " << radius << " Center: " << printVec3(center);
		}
		OUTPUT << std::endl;
	}
	OUTPUT << std::endl;
	return OUTPUT.str();
}

bool MapQuad::test(glm::vec4* planes, glm::mat4 vp) {//the culling process (planes length = 6, any less and there will be trouble)
	renderState = true;
	for (int i = 0; i < 6; i++) {
		glm::vec4 center(this->center, 1.f);
		float d = (planes[i].x * center.x) + (planes[i].y * center.y) + (planes[i].z * center.z) + planes[i].w;//determine distance between sphere and plane
		if (d < -radius) {//might have to be switched
			if (renderState) {
				cull();
			}
			return false;//the sphere is outside on of the planes, exit early
		}
	}
	renderState = true;
	if (children[0] != nullptr) {//if the quad has children, testing continues to them
		for (int i = 0; i < 4; i++) {
			children[i]->test(planes, vp);
		}
	}

	return false;
}

void MapQuad::update(float delta) {
	_mesh->uploadBufferData("m", model_matrix);
	if (children[0] != nullptr) {
		for (int i = 0; i < 4; i++) {
			children[i]->update(delta);
		}
	}
}

void MapQuad::cull() {
	renderState = false;
	if (children[0] != nullptr) {
		for (int i = 0; i < 4; i++) {
			children[i]->cull();
		}
	}
}

void MapQuad::render(GLenum drawMode) {
	if (renderState) {
		if (children[0] != nullptr) {
			for (int i = 0; i < 4; i++) {
				children[i]->render();
			}
		}
		else {
			texture->bind(0);
			normalTexture->bind(1);
			Entity::render(drawMode);
		}
	}
}

float MapQuad::calcHeightOfMesh(glm::vec3 position) {
	glm::vec3 p = pointOnMesh(position);
	return p.y;
}

glm::vec3 MapQuad::pointOnMesh(glm::vec3 position) {//
	if (isOutsideQuad(position)) {
		return glm::vec3(0.0f,1.0f,0.0f);
	}

	glm::vec3 normal = *selectNormal(position.x, position.z, 1.0f);

	int xa = nearbyintf(position.x);
	int ya = nearbyintf(position.z);

	glm::vec3 cp = glm::vec3((float)xa, *vertexes[xa][ya], (float)ya) - position;
	float t = glm::dot(cp, normal);
	float n = glm::length(normal)*glm::length(normal);
	glm::vec3 proj = (t / n) * normal;
	return glm::vec3(position.x, position.y + proj.y, position.z);

	/*for (int y = 0; y < height - 1; y++) {//brutal full mesh search (not finished)
		for (int x = 0; x < width - 1; x++) {
			glm::vec3 p = 
		}
	}*/
}

bool MapQuad::isOutsideQuad(glm::vec3 position) {
	return (position.x <= offsetX || position.x >= width - 1 || position.z <= offsetY || position.z >= height - 1);
}
glm::vec3 MapQuad::getCenter() {
	return center;
}


MapQuad::~MapQuad()
{
}
