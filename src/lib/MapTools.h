#ifndef MAPTOOLS_H
#define MAPTOOLS_H

#include <string>
#include <sstream>

#include "glm/glm.hpp"

#include "../entity/mapquad.hpp"

#include "SDL2/SDL.h"
#undef main
#include "SDL2/SDL_image.h"

using namespace std;
namespace MapTools {//tools used for generating the map and maintaining the MapQuads (will also contain the terrain generation functions, read from image to generate terrain etc)

	float*** vertexCopy(int startX, int startY, int width, int height, float*** vertexes) {//copy a part of the vertex array to create new MapQuads
		float*** v2 = new float**[width];
		for (int i = 0; i < width; i++) {
			v2[i] = new float*[height];
		}

		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				v2[x][y] = vertexes[x + startX][y + startY];
			}
		}
		return v2;
	}

	template <typename T>
	T*** pointerArrayCopy(int startX, int startY, int width, int height, T*** objects) {//general triple pointer copy function
		T*** v2 = new T**[width];
		for (int i = 0; i < width; i++) {
			v2[i] = new T*[height];
		}

		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				v2[x][y] = objects[x + startX][y + startY];
			}
		}
		return v2;
	}

	glm::vec3*** calculateNormals(float*** vertexes, int width, int height) {//makes a vector that has width = (mesh_width-1)*2 and height = mesh_height -1 that contains all normals for the mesh (may be moved to the MapQuad class)
		int Nwidth = (width - 1) * 2;//new vertex for storing normals has one less unit of height but is almost twice as wide to assign the triangles formed by the vertexes normals
		int Nheight = height-1;

		glm::vec3*** normals = new glm::vec3**[Nwidth];
		for (int i = 0; i < Nwidth; i++) {
			normals[i] = new glm::vec3*[Nheight];//new vertex for the normals
		}

		int nx = 0;
		int ny = 0;

		for (int y = 0; y < height-1; y++) {//double for-loop through the vertex array (-1 width and height because the main body takes + 1 from current value to calculate normals)

			for (int x = 0; x < width-1; x++) {//normals assignment process

				glm::vec3 p;//center point
				glm::vec3 v1;//vector 1
				glm::vec3 v2;//vector 2

				//uses cross product using the vertex array data to form the normals of a mesh.

				p =		glm::vec3(0 + x, *vertexes[0 + x][1 + y], 1 + y);//lower left of quad
				v1 =	glm::vec3(0 + x, *vertexes[0 + x][0 + y], 0 + y);
				v2 =	glm::vec3(1 + x, *vertexes[1 + x][1 + y], 1 + y);

				normals[nx][ny] = new glm::vec3(glm::cross(v1 - p, v2 - p));
				*normals[nx][ny] = glm::normalize(*normals[nx][ny]);

				nx++;

				p =		glm::vec3(1 + x, *vertexes[1 + x][0 + y], 0 + y);//upper right of quad

				normals[nx][ny] = new glm::vec3(glm::cross(v1 - p, v2 - p));
				*normals[nx][ny] = glm::normalize(*normals[nx][ny]);

				nx++;
			}
			nx=0;//reset normal array x position
			ny++;//jump down one y level on the normal array
		}
		return normals;
	}

	Uint32 getPixel(SDL_Surface *surface, int x, int y)//get pixel value from the SDL_surface
	{
		int bpp = surface->format->BytesPerPixel;

		Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

		switch (bpp) {
		case 1:
			return *p;
		break;

		case 2:
			return *(Uint16 *)p;
		break;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] | p[1] | p[2];
			else
				return p[0] | p[1] | p[2];
		break;

		case 4:
			return *(Uint32 *)p;
		break;
		}
	}

	float*** mapBMPreader(string fileLocation) {//read heightmap from BMP file (!!DOES NOT LIKE IMAGES WHERE HEIGHT != WIDTH!!)
		SDL_Surface * image = SDL_LoadBMP(fileLocation.c_str());
		cout << "w:" << image->w << endl;
		cout << "h: " << image->h << endl;

		float*** map = new float**[image->w];
		for (int i = 0; i < image->h; i++) {
			map[i] = new float*[image->h];
		}
		cout << getPixel(image, 0, 1) << endl;
		int bpp = image->format->BytesPerPixel;
		for (int x = 0; x < image->w; x++) {
			for (int y = 0; y < image->h; y++) {
				map[x][y] = new float((getPixel(image, x, y) / 30.5f) - 10.f);
			}
		}
		return map;
	}

	float*** meshInterpolate(string file, int width, int height, float*** vertexes) {//potential future dunction to interpolate different heightmaps
		float*** v1 = mapBMPreader(file);
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

			}
		}
		return nullptr;
	}

	string printVec3(glm::vec3 &in) {
		stringstream OUTPUT;
		OUTPUT << "(" << in.x << ", " << in.y << ", " << in.z << ")";
		return OUTPUT.str();
	}

	string printVec4(glm::vec4 &in) {
		stringstream OUTPUT;
		OUTPUT << "(" << in.x << ", " << in.y << ", " << in.z << ", " << in.w << ")";
		return OUTPUT.str();
	}

	MapQuad* quadtreeSplit(MapQuad* parent, int depth, int height, int width) { //operates with (2^x)+1 numbers, even numbers break the quad generation (unsure about result when height != width)
		MapQuad* children[4];

		int w2 = (width / 2)+1;//define where the middle grounds are (MapQuad meshes need to share some vectors to form a complete mesh when being rendered)
		int h2 = (height / 2)+1;

		children[0] = new MapQuad(parent, vertexCopy(0, 0, w2, h2, parent->getVertexes()), calculateNormals(vertexCopy(0, 0, w2, h2, parent->getVertexes()), w2, h2), h2, w2, 0 + parent->getOffsetX(), 0 + parent->getOffsetY());//assign children their vertexes, in a 01 configuration
		children[1] = new MapQuad(parent, vertexCopy(w2-1, 0, w2, h2, parent->getVertexes()), calculateNormals(vertexCopy(w2 - 1, 0, w2, h2, parent->getVertexes()), w2, h2), h2, w2, w2 + parent->getOffsetX() - 1, 0 + parent->getOffsetY());						  //  23
		children[2] = new MapQuad(parent, vertexCopy(0, h2-1, w2, h2, parent->getVertexes()), calculateNormals(vertexCopy(0, h2 - 1, w2, h2, parent->getVertexes()), w2, h2), h2, w2, 0 + parent->getOffsetX(), h2 + parent->getOffsetY() - 1);
		children[3] = new MapQuad(parent, vertexCopy(w2-1, h2-1, w2, h2, parent->getVertexes()), calculateNormals(vertexCopy(w2 - 1, h2 - 1, w2, h2, parent->getVertexes()),w2,h2), h2, w2, w2 + parent->getOffsetX() - 1, h2 + parent->getOffsetY() - 1);

		parent->setChildren(children);
		depth--;

		if (depth != 0) {//breaks recursion at desired depth
			quadtreeSplit(children[0], depth, h2, w2);
			quadtreeSplit(children[1], depth, h2, w2);
			quadtreeSplit(children[2], depth, h2, w2);
			quadtreeSplit(children[3], depth, h2, w2);
		}
		return parent;//only used when called from outside the recursion (assigning main parent)
	}

	glm::vec4 planeNormalization(glm::vec4 plane) {//only uses the first three components (x, y, z) of the vec4 to normalize, only useful for planes (w is used as d, in ax + by + cz + d format)
		float n = 1.0f / sqrt((plane.x * plane.x) + (plane.y * plane.y) + (plane.z * plane.z));

		return (plane * n);
	}

	glm::vec4* calculateFrustrumPlanes(glm::mat4 mvp) {//take mvp matrix and extract planes for frustrum culling (the planes should then be saved somewhere (inside the MapQuads, as pointers maybe?))
		glm::vec4* planes = new glm::vec4[6];

		mvp = glm::transpose(mvp);

		//Left clipping plane
		planes[0].x = mvp[0][0] + mvp[3][0];
		planes[0].y = mvp[0][1] + mvp[3][1];
		planes[0].z = mvp[0][2] + mvp[3][2];
		planes[0].w = mvp[0][3] + mvp[3][3];
		//Right clipping plane.
		planes[1].x = -mvp[0][0] + mvp[3][0];
		planes[1].y = -mvp[0][1] + mvp[3][1];
		planes[1].z = -mvp[0][2] + mvp[3][2];
		planes[1].w = -mvp[0][3] + mvp[3][3];
		//Bottom clipping plane.
		planes[2].x = mvp[1][0] + mvp[3][0];
		planes[2].y = mvp[1][1] + mvp[3][1];
		planes[2].z = mvp[1][2] + mvp[3][2];
		planes[2].w = mvp[1][3] + mvp[3][3];
		//Top clipping plane.
		planes[3].x = -mvp[1][0] + mvp[3][0];
		planes[3].y = -mvp[1][1] + mvp[3][1];
		planes[3].z = -mvp[1][2] + mvp[3][2];
		planes[3].w = -mvp[1][3] + mvp[3][3];
		//Far clipping plane.
		planes[4].x = -mvp[2][0] + mvp[3][0];
		planes[4].y = -mvp[2][1] + mvp[3][1];
		planes[4].z = -mvp[2][2] + mvp[3][2];
		planes[4].w = -mvp[2][3] + mvp[3][3];
		//Near clipping plane.
		planes[5].x = mvp[2][0] + mvp[3][0];
		planes[5].y = mvp[2][1] + mvp[3][1];
		planes[5].z = mvp[2][2] + mvp[3][2];
		planes[5].w = mvp[2][3] + mvp[3][3];

		for (int i = 0; i < 6; i++) {
			planeNormalization(planes[i]);
		}

		return planes;
	}
}
#endif
