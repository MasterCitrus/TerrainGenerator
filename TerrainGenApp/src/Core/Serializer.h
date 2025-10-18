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
	static SaveData DeserializeV1(std::ifstream& file);

	// Header
	static std::string GetHeaderString(FileHeader header);
	static FileHeader GetHeaderVersion(const std::string& header);

	// String read/write
	static std::string ReadString(std::ifstream& file);
	static void WriteString(std::ofstream& file, const std::string& source);

	// Vector read/write
	static Math::Vec2 ReadVec2(std::ifstream& file);
	static void WriteVec2(std::ofstream& file, const Math::Vec2& source);
	static Math::Vec3 ReadVec3(std::ifstream& file);
	static void WriteVec3(std::ofstream& file, const Math::Vec3& source);

	// Curve Key read/write
	static CurveKey ReadCurveKey(std::ifstream& file);
	static void WriteCurveKey(std::ofstream& file, const CurveKey& source);
};