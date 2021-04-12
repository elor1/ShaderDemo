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
		gD3DContext->PSSetShaderResources(i, 1, &textures[i]->textureSRV);
	}

	model->Render();
}
