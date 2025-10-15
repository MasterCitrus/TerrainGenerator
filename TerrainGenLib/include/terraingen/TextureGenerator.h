#pragma once

#include "types/Defines.h"

class TextureGenerator
{
public:

	TextureData GenerateTextureFromNoise(const HeightData& data);
	TextureData GenerateTextureFromColour(const ColourData& data);

private:

};