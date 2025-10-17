#pragma once

#include "terraingen/types/TerrainData.h"

#include <filesystem>

struct aiMesh;

enum class FileType : uint8_t
{
	OBJ = 0,
	FBX,
	FileType_Count
};

class TerrainExporter
{
public:
	void Export(std::filesystem::path exportPath, const TerrainData& data, FileType type = FileType::OBJ, bool textures = true);

private:
	TextureData FlipTexture(const TextureData& data, unsigned int size, unsigned int channels);

	void ConvertToAIMesh(const MeshData& mesh, aiMesh* outMesh);
};