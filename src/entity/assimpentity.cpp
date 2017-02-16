#include "assimpentity.hpp"

#include <exception>
#include <cstdio>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cstdlib>

#include "../engine.hpp"

bool hasVertexColors(aiMesh* mesh, unsigned int pIndex) {
	if (pIndex >= AI_MAX_NUMBER_OF_COLOR_SETS)
		return false;
	else
		return mesh->mColors[pIndex] != NULL && mesh->mNumVertices > 0;
}

bool hasTextureCoords(aiMesh* mesh, unsigned int pIndex) {
	if (pIndex >= AI_MAX_NUMBER_OF_TEXTURECOORDS)
		return false;
	else
		return mesh->mTextureCoords[pIndex] != NULL && mesh->mNumVertices > 0;
}

AssimpEntity::AssimpEntity(std::shared_ptr<ShaderProgram> program, const std::string& filename)
	: Entity(_getModel(program, filename)), _texture(_getTexture(filename)) {}

AssimpEntity::~AssimpEntity() {}

void AssimpEntity::render(GLenum drawMode) {
	_texture->bind(0);
	Entity::render(drawMode);
}

std::shared_ptr<Mesh> AssimpEntity::_getModel(std::shared_ptr<ShaderProgram> program, const std::string& filename) {
	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
	const aiScene* scene = importer.ReadFile(filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
																											 aiProcess_SortByPType | aiProcess_GenNormals | aiProcess_FlipUVs);
	if (!scene) {
		fprintf(stderr, "Could not load model %s\n", filename.c_str());
		throw std::exception();
	}

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

unsigned int counterVertices = 0;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		bool hasColors = hasVertexColors(mesh, 0);
		bool hasUV = hasTextureCoords(mesh, 0);
		for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
			Vertex vertex;

			aiVector3D p = mesh->mVertices[j];
			vertex.position = glm::vec3{p.x, p.y, p.z};

			p = mesh->mNormals[j];
			vertex.normal = glm::vec3{p.x, p.y, p.z};

			if (hasColors) {
				aiColor4D c = mesh->mColors[0][j];
				vertex.color = glm::vec3{c.r, c.g, c.b};
			} else
				vertex.color = glm::vec3{1.f, 1.f, 1.f};

			if (hasUV) {
				aiVector3D uv = mesh->mTextureCoords[0][j];
				vertex.uv = glm::vec2{uv.x, uv.y};
			} else
				vertex.uv = glm::vec2{1.f, 1.f};

			vertices.push_back(vertex);
		}

		for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
			unsigned int* face = mesh->mFaces[j].mIndices;
			indices.push_back(counterVertices + face[0]);
			indices.push_back(counterVertices + face[1]);
			indices.push_back(counterVertices + face[2]);
		}
		counterVertices += mesh->mNumVertices;
	}

	return std::make_shared<Mesh>(program, vertices, indices);
}

std::shared_ptr<Texture> AssimpEntity::_getTexture(const std::string& filename) {
	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
	const aiScene* scene = importer.ReadFile(filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
																											 aiProcess_SortByPType | aiProcess_GenNormals | aiProcess_FlipUVs);
	if (!scene) {
		fprintf(stderr, "Could not load model %s\n", filename.c_str());
		throw std::exception();
	}

	if (scene->mNumMaterials < 0)
		return Engine::getInstance().getTextureManager()->getTexture(""); // Error texture
	const aiMaterial* pMaterial = scene->mMaterials[0];
	if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) == 0)
		return Engine::getInstance().getTextureManager()->getTexture(""); // Error texture

	aiString path;

	if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) != AI_SUCCESS)
		return Engine::getInstance().getTextureManager()->getTexture(""); // Error texture

	if (path.data[0] == '*') {
		unsigned int id = atoi(path.data + 1);
		printf("Embedded texture: %d(%s)\n", id, path.data);
		if (scene->mNumTextures < id)
			return Engine::getInstance().getTextureManager()->getTexture(""); // Error texture
		aiTexture* tex = scene->mTextures[id];

		printf("Texture: \n");
		printf("\tmWidth: %d\n", tex->mWidth);
		printf("\tmHeight: %d\n", tex->mHeight);
		printf("\tachFormatHint: %s\n", tex->achFormatHint);
		printf("\tpcData: %p\n", (void*)tex->pcData);
		if (tex->mHeight)
			return std::make_shared<Texture>(tex->mWidth, tex->mHeight, (void*)tex->pcData);
		else
			return std::make_shared<Texture>(tex->achFormatHint, (void*)tex->pcData, tex->mWidth);
	} else {
		std::string fullPath = filename.substr(0, filename.find_last_of("/\\")) + path.data; // TODO: fix path
		printf("External texture: %s\n", fullPath.c_str());
		return Engine::getInstance().getTextureManager()->getTexture(fullPath);
	}
}
