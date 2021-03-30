#include "Texture.h"

#include "GraphicsHelpers.h"

Texture::Texture(std::string filename)
{
	fileName = filename;
}

bool Texture::Load()
{
	if (!LoadTexture(fileName, &diffuseSpecularMap, &diffuseSpecularMapSRV))
	{
		gLastError = "Error loading texture " + fileName;
		return false;
	}
	return true;
}
