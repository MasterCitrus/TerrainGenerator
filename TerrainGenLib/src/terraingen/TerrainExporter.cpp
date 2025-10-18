#include "terraingen/TerrainExporter.h"

#include <assimp/scene.h>
#include <assimp/Exporter.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void TerrainExporter::Export(std::filesystem::path exportPath, const TerrainData& data, FileType type, bool textures)
{
	aiScene* scene = new aiScene();
	aiMesh* mesh = new aiMesh();
	mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;

	ConvertToAIMesh(data.meshData, mesh);

	scene->mNumMeshes = 1;
	scene->mMeshes = new aiMesh*[scene->mNumMeshes];
	scene->mMeshes[0] = mesh;

	aiNode* rootNode = new aiNode();
	rootNode->mName = "Terrain";
	rootNode->mNumMeshes = 1;
	rootNode->mMeshes = new unsigned int[rootNode->mNumMeshes];
	rootNode->mMeshes[0] = 0;
	scene->mRootNode = rootNode;

	scene->mNumMaterials = 1;
	scene->mMaterials = new aiMaterial*[1];
	scene->mMaterials[0] = new aiMaterial();
	mesh->mMaterialIndex = 0;

	Assimp::Exporter exporter;
	Assimp::ExportProperties properties;

	switch (type)
	{
		case FileType::FBX:
			exporter.Export(scene, "fbx", exportPath.string());
			break;
		case FileType::OBJ:
			exporter.Export(scene, "obj", exportPath.string());
			break;
		default:
			break;
	}

	std::string path = exportPath.string().substr(0, exportPath.string().find_last_of('.'));
	std::string heightMap = path + "_Height.png";
	std::string colourMap = path + "_Colour.png";

	if (textures)
	{
		if (data.usesFalloff)
		{
			stbi_write_png(heightMap.c_str(), data.size, data.size, 1, FlipTexture(data.heightTexture, data.size, 1).data(), data.size);
			stbi_write_png(colourMap.c_str(), data.size, data.size, 3, FlipTexture(data.colourTexture, data.size, 3).data(), data.size * 3);
		}
		else
		{
			stbi_write_png(heightMap.c_str(), data.size, data.size, 1, FlipTexture(data.noiseTexture, data.size, 1).data(), data.size);
			stbi_write_png(colourMap.c_str(), data.size, data.size, 3, FlipTexture(data.colourTexture, data.size, 3).data(), data.size * 3);
		}
	}

	delete scene;
}

TextureData TerrainExporter::FlipTexture(const TextureData& data, unsigned int size, unsigned int channels)
{
	unsigned int rowSize = size * channels;
	TextureData tempCopy = data;
	TextureData row(rowSize);

	for (unsigned int i = 0; i < size / 2; i++)
	{
		unsigned char* rowTop = tempCopy.data() + i * rowSize;
		unsigned char* rowBottom = tempCopy.data() + (size - i - 1) * rowSize;

		std::memcpy(tempCopy.data(), rowTop, rowSize);
		std::memcpy(rowTop, rowBottom, rowSize);
		std::memcpy(rowBottom, tempCopy.data(), rowSize);
	}

	return tempCopy;
}

void TerrainExporter::ConvertToAIMesh(const MeshData& mesh, aiMesh* outMesh)
{
	outMesh->mNumVertices = mesh.vertices.size();
	outMesh->mNumFaces = mesh.indices.size() / 3;
	outMesh->mFaces = new aiFace[outMesh->mNumFaces];
	outMesh->mVertices = new aiVector3D[outMesh->mNumVertices];
	outMesh->mNormals = new aiVector3D[outMesh->mNumVertices];
	outMesh->mTextureCoords[0] = new aiVector3D[outMesh->mNumVertices];
	outMesh->mNumUVComponents[0] = 2;

	for (unsigned int i = 0; i < mesh.vertices.size(); i++)
	{
		const Vertex& v = mesh.vertices[i];
		outMesh->mVertices[i] = aiVector3D(v.pos.x, v.pos.y, v.pos.z);
		outMesh->mNormals[i] = aiVector3D(v.norm.x, v.norm.y, v.norm.z);
		outMesh->mTextureCoords[0][i] = aiVector3D(v.uv.x, v.uv.y, 0.0f);
	}

	for (unsigned int i = 0; i < outMesh->mNumFaces; i++)
	{
		aiFace& face = outMesh->mFaces[i];
		face.mNumIndices = 3;
		face.mIndices = new unsigned int[3]
			{
					mesh.indices[i * 3 + 0],
					mesh.indices[i * 3 + 1],
					mesh.indices[i * 3 + 2]
			};
	}
}
