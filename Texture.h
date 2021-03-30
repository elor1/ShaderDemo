#pragma once
#include <d3d11.h>
#include <string>

class Texture
{
public:
	Texture(std::string filename);
	std::string fileName;
	ID3D11Resource* diffuseSpecularMap;
	ID3D11ShaderResourceView* diffuseSpecularMapSRV;

	bool Load();
};

