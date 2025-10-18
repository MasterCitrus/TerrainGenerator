#include "Serializer.h"

void Serializer::Serialize(const SaveData& data, const std::filesystem::path& path)
{
    // Version 1 of .tgen file
    std::ofstream file;
    file.open(path.string(), std::ios::binary);

    if (!file) return;

    std::string header = GetHeaderString(FileHeader::TGENV1);

    // Header
    WriteString(file, header);

    // Region data
    uint16_t numRegions = static_cast<uint16_t>(data.regions.size());
    file.write(reinterpret_cast<const char*>(&numRegions), sizeof(numRegions));

    for (int i = 0; i < data.regions.size(); i++)
    {
        WriteString(file, data.regions[i].name);
        WriteVec3(file, data.regions[i].colour);
        file.write(reinterpret_cast<const char*>(&data.regions[i].height), sizeof(float));
    }

    // Curve data
    uint16_t numKeys = static_cast<uint16_t>(data.heightCurve.AmountOfKeys());
    file.write(reinterpret_cast<const char*>(&numKeys), sizeof(numKeys));

    for (const auto& key : data.heightCurve.Keys())
    {
        WriteCurveKey(file, key);
    }

    // Other data
    WriteVec2(file, data.offset);

    file.write(reinterpret_cast<const char*>(&data.seed), sizeof(data.seed));
    file.write(reinterpret_cast<const char*>(&data.lacunarity), sizeof(float));
    file.write(reinterpret_cast<const char*>(&data.persistence), sizeof(float));
    file.write(reinterpret_cast<const char*>(&data.noiseScale), sizeof(float));
    file.write(reinterpret_cast<const char*>(&data.octaves), sizeof(data.octaves));
    file.write(reinterpret_cast<const char*>(&data.heightMultiplier), sizeof(float));

    file.close();
}

SaveData Serializer::Deserialize(const std::filesystem::path& path)
{
    std::ifstream file;
    file.open(path.string(), std::ios::binary);

    if (!file) return SaveData();

    std::string header = ReadString(file);

    FileHeader fileVersion = GetHeaderVersion(header);

    switch (fileVersion)
    {
        case FileHeader::TGENV1:
            return DeserializeV1(file);
        default:
            break;
    }

    return SaveData();
}

std::string Serializer::GetHeaderString(FileHeader header)
{
    switch (header)
    {
        case FileHeader::TGENV1:
            return std::string("TGENV1");
        default:
            break;
    }
    return std::string("Invalid Header");
}

FileHeader Serializer::GetHeaderVersion(const std::string& header)
{
    if (header == "TGENV1")
    {
        return FileHeader::TGENV1;
    }
    else
    {
        return FileHeader::FileHeader_COUNT;
    }
}

std::string Serializer::ReadString(std::ifstream& file)
{
    uint16_t len;
    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string s(len, '\0');
    file.read(s.data(), len);

    return s;
}

void Serializer::WriteString(std::ofstream& file, const std::string& source)
{
    uint16_t len = static_cast<uint16_t>(source.size());
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(source.data(), len);
}

Math::Vec2 Serializer::ReadVec2(std::ifstream& file)
{
    Math::Vec2 v;
    file.read(reinterpret_cast<char*>(&v.x), sizeof(float));
    file.read(reinterpret_cast<char*>(&v.y), sizeof(float));

    return v;
}

void Serializer::WriteVec2(std::ofstream& file, const Math::Vec2& source)
{
    file.write(reinterpret_cast<const char*>(&source.x), sizeof(float));
    file.write(reinterpret_cast<const char*>(&source.y), sizeof(float));
}

Math::Vec3 Serializer::ReadVec3(std::ifstream& file)
{
    Math::Vec3 v;
    file.read(reinterpret_cast<char*>(&v.x), sizeof(float));
    file.read(reinterpret_cast<char*>(&v.y), sizeof(float));
    file.read(reinterpret_cast<char*>(&v.z), sizeof(float));

    return v;
}

void Serializer::WriteVec3(std::ofstream& file, const Math::Vec3& source)
{
    file.write(reinterpret_cast<const char*>(&source.x), sizeof(float));
    file.write(reinterpret_cast<const char*>(&source.y), sizeof(float));
    file.write(reinterpret_cast<const char*>(&source.z), sizeof(float));
}

CurveKey Serializer::ReadCurveKey(std::ifstream& file)
{
    CurveKey key;
    file.read(reinterpret_cast<char*>(&key.time), sizeof(float));
    file.read(reinterpret_cast<char*>(&key.value), sizeof(float));
    file.read(reinterpret_cast<char*>(&key.inTangent), sizeof(float));
    file.read(reinterpret_cast<char*>(&key.outTangent), sizeof(float));

    uint8_t selected;
    file.read(reinterpret_cast<char*>(&selected), sizeof(selected));
    key.selected = selected != 0;
    return key;
}

void Serializer::WriteCurveKey(std::ofstream& file, const CurveKey& source)
{
    file.write(reinterpret_cast<const char*>(&source.time), sizeof(float));
    file.write(reinterpret_cast<const char*>(&source.value), sizeof(float));
    file.write(reinterpret_cast<const char*>(&source.inTangent), sizeof(float));
    file.write(reinterpret_cast<const char*>(&source.outTangent), sizeof(float));

    uint8_t selected = source.selected ? 1 : 0;
    file.write(reinterpret_cast<const char*>(&selected), sizeof(selected));
}

SaveData Serializer::DeserializeV1(std::ifstream& file)
{
    SaveData data;

    // Region data
    uint16_t numRegions;
    file.read(reinterpret_cast<char*>(&numRegions), sizeof(numRegions));

    data.regions.resize(numRegions);
    for (auto& region : data.regions)
    {
        region.name = ReadString(file);
        region.colour = ReadVec3(file);
        file.read(reinterpret_cast<char*>(&region.height), sizeof(float));
    }

    // Curve data
    uint16_t numKeys;
    file.read(reinterpret_cast<char*>(&numKeys), sizeof(numKeys));

    Curve curve;
    curve.ClearKeys();
    for (int i = 0; i < numKeys; i++)
    {
        curve.AddKey(ReadCurveKey(file));
    }

    data.heightCurve = curve;

    // Other data
    data.offset = ReadVec2(file);

    file.read(reinterpret_cast<char*>(&data.seed), sizeof(data.seed));
    file.read(reinterpret_cast<char*>(&data.lacunarity), sizeof(float));
    file.read(reinterpret_cast<char*>(&data.persistence), sizeof(float));
    file.read(reinterpret_cast<char*>(&data.noiseScale), sizeof(float));
    file.read(reinterpret_cast<char*>(&data.octaves), sizeof(data.octaves));
    file.read(reinterpret_cast<char*>(&data.heightMultiplier), sizeof(float));

    file.close();

    data.valid = true;

    return data;
}
