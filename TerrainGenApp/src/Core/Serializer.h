#pragma once

#include "SaveData.h"

#include <filesystem>
#include <fstream>
#include <string>

class Serializer
{
public:

	static void Serialize(const SaveData& data, const std::filesystem::path& path);
	static SaveData Deserialize(const std::filesystem::path& path);

private:
	static std::string GetHeaderString(FileHeader header);
	static FileHeader GetHeaderVersion(const std::string& header);

	static SaveData DeserializeV1(std::ifstream& file);
};