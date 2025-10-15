#include "terraingen/TextureGenerator.h"

#include <cmath>

TextureData TextureGenerator::GenerateTextureFromNoise(const HeightData& data)
{
    unsigned int size = data.size();

	TextureData heightTextureData(size * size);


    int k = 0;
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            heightTextureData[k] = std::floor(data[y][x] * 255);
            k++;
        }
    }

	return heightTextureData;
}

TextureData TextureGenerator::GenerateTextureFromColour(const ColourData& data)
{
    unsigned int size = data.size();

    TextureData colourTextureData(size * 3);

    for (int i = 0; i < size; i++)
    {
        colourTextureData[i * 3 + 0] = std::floor(data[i].x * 255);
        colourTextureData[i * 3 + 1] = std::floor(data[i].y * 255);
        colourTextureData[i * 3 + 2] = std::floor(data[i].z * 255);
    }

    return colourTextureData;
}
