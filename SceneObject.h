#pragma once
#include "Model.h"
#include <vector>

#include "Texture.h"

class SceneObject
{
public:
	SceneObject(Model* Model, Texture* Texture, ID3D11VertexShader* VertexShader, ID3D11PixelShader* PixelShader,
	            ID3D11BlendState* BlendState, ID3D11RasterizerState* RasterizerState,
	            ID3D11DepthStencilState* DepthStencilState, ID3D11SamplerState* SamplerState, bool control);
	~SceneObject();
	Model* ObjectModel();
	std::vector<Texture*> Textures();
	void AddTexture(Texture* texture);
	bool IsControllable();
	ID3D11VertexShader* VertexShader();
	ID3D11PixelShader* PixelShader();
	ID3D11BlendState* BlendState();
	ID3D11RasterizerState* RasterizerState();
	ID3D11DepthStencilState* DepthStencilState();
	ID3D11SamplerState** SamplerState();
	virtual void Render();

private:
	Model* model;
	std::vector<Texture*> textures;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11BlendState* blendState;
	ID3D11RasterizerState* rasterizerState;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11SamplerState* samplerState;

	bool isControllable = false;

	
};

