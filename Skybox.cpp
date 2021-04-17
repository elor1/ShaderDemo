#include "Skybox.h"
void Skybox::Render()
{
	gD3DContext->VSSetShader(VertexShader(), nullptr, 0);
	gD3DContext->PSSetShader(PixelShader(), nullptr, 0);
	gD3DContext->OMSetBlendState(BlendState(), nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(DepthStencilState(), 0);
	gD3DContext->RSSetState(RasterizerState());
	gD3DContext->PSSetSamplers(0, 1, SamplerState());

	ObjectModel()->Render();
}