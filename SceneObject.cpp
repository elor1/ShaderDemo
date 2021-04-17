#include "SceneObject.h"

SceneObject::SceneObject(Model* Model, Texture* Texture, ID3D11VertexShader* VertexShader,
	ID3D11PixelShader* PixelShader, ID3D11BlendState* BlendState, ID3D11RasterizerState* RasterizerState,
	ID3D11DepthStencilState* DepthStencilState, ID3D11SamplerState* SamplerState, bool control)
{
	model = Model;
	textures.push_back(Texture);
	vertexShader = VertexShader;
	pixelShader = PixelShader;
	blendState = BlendState;
	rasterizerState = RasterizerState;
	depthStencilState = DepthStencilState;
	samplerState = SamplerState;
	isControllable = control;
}

SceneObject::~SceneObject()
{
	delete model; model = nullptr;
	for (auto texture : textures)
	{
		delete texture; texture = nullptr;
	}
}

Model* SceneObject::ObjectModel()
{
	return model;
}

std::vector<Texture*> SceneObject::Textures()
{
	return textures;
}

void SceneObject::AddTexture(Texture* texture)
{
	textures.push_back(texture);
}

bool SceneObject::IsControllable()
{
	return isControllable;
}

ID3D11VertexShader* SceneObject::VertexShader()
{
	return vertexShader;
}

ID3D11PixelShader* SceneObject::PixelShader()
{
	return pixelShader;
}

ID3D11BlendState* SceneObject::BlendState()
{
	return  blendState;
}

ID3D11RasterizerState* SceneObject::RasterizerState()
{
	return rasterizerState;
}

ID3D11DepthStencilState* SceneObject::DepthStencilState()
{
	return depthStencilState;
}

ID3D11SamplerState** SceneObject::SamplerState()
{
	return &samplerState;
}

void SceneObject::Render()
{
	gD3DContext->VSSetShader(vertexShader, nullptr, 0);
	gD3DContext->PSSetShader(pixelShader, nullptr, 0);
	gD3DContext->OMSetBlendState(blendState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(depthStencilState, 0);
	gD3DContext->RSSetState(rasterizerState);
	gD3DContext->PSSetSamplers(0, 1, &samplerState);

	for (int i = 0; i < textures.size(); i++)
	{
		gD3DContext->PSSetShaderResources(i, 1, textures[i]->TextureSRV());
	}

	model->Render();
}
