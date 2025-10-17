#include "Serializer.h"

void Serializer::Serialize(const SaveData& data, const std::filesystem::path& path)
{
    // Version 1 of .tgen file
    std::ofstream file;
    file.open(path.string(), std::ios::binary);

    auto header = GetHeaderString(FileHeader::TGENV1);

    uint8_t len = header.length();
    uint8_t padding = 10 - len;

    std::vector<char> null(padding, '\0');

    // Header with version + padding
    file.write(header.data(), len);
    file.write(null.data(), padding);

    // Region data
    uint16_t numRegions = static_cast<uint16_t>(data.regions.size());
    file.write(reinterpret_cast<const char*>(&numRegions), sizeof(numRegions));

    for (int i = 0; i < data.regions.size(); i++)
    {
        uint8_t strLen = data.regions[i].name.length();
        file.write(reinterpret_cast<const char*>(&strLen), sizeof(strLen));
        file.write(data.regions[i].name.data(), strLen);
        file.write(reinterpret_cast<const char*>(&data.regions[i].colour.x), sizeof(float));
        file.write(reinterpret_cast<const char*>(&data.regions[i].colour.y), sizeof(float));
        file.write(reinterpret_cast<const char*>(&data.regions[i].colour.z), sizeof(float));
        file.write(reinterpret_cast<const char*>(&data.regions[i].height), sizeof(float));
    }

    // Curve data
    uint16_t numCurves = static_cast<uint16_t>(data.heightCurve.AmountOfKeys());
    file.write(reinterpret_cast<const char*>(&numCurves), sizeof(numCurves));

    for (const auto& key : data.heightCurve.Keys())
    {
        file.write(reinterpret_cast<const char*>(&key.time), sizeof(float));
        file.write(reinterpret_cast<const char*>(&key.value), sizeof(float));
        file.write(reinterpret_cast<const char*>(&key.inTangent), sizeof(float));
        file.write(reinterpret_cast<const char*>(&key.outTangent), sizeof(float));

        uint8_t selected = key.selected ? 1 : 0;
        file.write(reinterpret_cast<const char*>(&selected), sizeof(selected));
    }

    // Other data
    file.write(reinterpret_cast<const char*>(&data.offset.x), sizeof(float));
    file.write(reinterpret_cast<const char*>(&data.offset.y), sizeof(float));

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

    std::string header;

    file.read(header.data(), 10);

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

SaveData Serializer::DeserializeV1(std::ifstream& file)
{
    SaveData data;

    // Region data
    uint16_t numRegions;
    file.read(reinterpret_cast<char*>(&numRegions), sizeof(numRegions));

    data.regions.resize(numRegions);
    for (int i = 0; i < data.regions.size(); i++)
    {
        uint8_t strLen;
        file.read(reinterpret_cast<char*>(&strLen), sizeof(strLen));
        std::string regionName(strLen, '\0');
        file.read(regionName.data(), strLen);
        data.regions[i].name = regionName;
        file.read(reinterpret_cast<char*>(&data.regions[i].colour.x), sizeof(float));
        file.read(reinterpret_cast<char*>(&data.regions[i].colour.y), sizeof(float));
        file.read(reinterpret_cast<char*>(&data.regions[i].colour.z), sizeof(float));
        file.read(reinterpret_cast<char*>(&data.regions[i].height), sizeof(float));
    }

    // Curve data
    uint16_t numCurves;
    file.read(reinterpret_cast<char*>(&numCurves), sizeof(numCurves));

    Curve curve;
    curve.ClearKeys();
    for (int i = 0; i < numCurves; i++)
    {
        CurveKey key;
        file.read(reinterpret_cast<char*>(&key.time), sizeof(float));
        file.read(reinterpret_cast<char*>(&key.value), sizeof(float));
        file.read(reinterpret_cast<char*>(&key.inTangent), sizeof(float));
        file.read(reinterpret_cast<char*>(&key.outTangent), sizeof(float));

        uint8_t selected;
        file.read(reinterpret_cast<char*>(selected), sizeof(selected));
        key.selected = static_cast<bool>(selected);

        curve.AddKey(key);
    }

    data.heightCurve = curve;

    // Other data
    file.read(reinterpret_cast<char*>(&data.offset.x), sizeof(float));
    file.read(reinterpret_cast<char*>(&data.offset.y), sizeof(float));

    file.read(reinterpret_cast<char*>(&data.seed), sizeof(data.seed));
    file.read(reinterpret_cast<char*>(&data.lacunarity), sizeof(float));
    file.read(reinterpret_cast<char*>(&data.persistence), sizeof(float));
    file.read(reinterpret_cast<char*>(&data.noiseScale), sizeof(float));
    file.read(reinterpret_cast<char*>(&data.octaves), sizeof(data.octaves));
    file.read(reinterpret_cast<char*>(&data.heightMultiplier), sizeof(float));

    file.close();

    return data;
}
