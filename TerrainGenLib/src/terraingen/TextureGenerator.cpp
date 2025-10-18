#include "terraingen/TextureGenerator.h"
#include "terraingen/types/math/Utils.h"

#include <cmath>
#include <algorithm>

TextureData TextureGenerator::GenerateTextureFromNoise(const HeightData& data)
{
    unsigned int size = data.size();

	TextureData noiseData(size * size);


    int k = 0;
    for (unsigned int y = 0; y < size; y++)
    {
        for (unsigned int x = 0; x < size; x++)
        {
            noiseData[k] = std::floor(data[y][x] * 255.0f);
            k++;
        }
    }

	return noiseData;
}

TextureData TextureGenerator::GenerateTextureFromColour(const ColourData& data)
{
    unsigned int size = data.size();

    TextureData colourTextureData(size * 3);

    for (unsigned int i = 0; i < size; i++)
    {
        colourTextureData[i * 3 + 0] = std::floor(data[i].x * 255.0f);
        colourTextureData[i * 3 + 1] = std::floor(data[i].y * 255.0f);
        colourTextureData[i * 3 + 2] = std::floor(data[i].z * 255.0f);
    }

    return colourTextureData;
}
