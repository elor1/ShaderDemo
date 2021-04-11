#include "Texture.h"

#include "GraphicsHelpers.h"

Texture::Texture(std::string filename)
{
	fileName = filename;
}

Texture::~Texture()
{
	if (textureResource) textureResource->Release();
	if (textureSRV) textureSRV->Release();
}

bool Texture::Load()
{
	if (!LoadTexture(fileName, &textureResource, &textureSRV))
	{
		gLastError = "Error loading texture " + fileName;
		return false;
	}
	return true;
}
