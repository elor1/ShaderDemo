#pragma once
#include <d3d11.h>
#include <string>

class Texture
{
public:
	Texture(std::string filename);
	~Texture();
	
	std::string fileName;
	ID3D11Resource* textureResource;
	ID3D11ShaderResourceView* textureSRV;

	bool Load();
};

