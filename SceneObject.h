#pragma once
#include "Model.h"
#include <vector>

#include "Texture.h"

class SceneObject
{
public:
	SceneObject(Model* Model, Texture* Texture, ID3D11VertexShader* VertexShader, ID3D11PixelShader* PixelShader,
	            ID3D11BlendState* BlendState, ID3D11RasterizerState* RasterizerState,
	            ID3D11DepthStencilState* DepthStencilState, ID3D11SamplerState* SamplerState);
	~SceneObject();
	
	Model* model;
	std::vector<Texture*> textures;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11BlendState* blendState;
	ID3D11RasterizerState* rasterizerState;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11SamplerState* samplerState;

	void Render();
};

