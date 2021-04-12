#include "Skybox.h"
void Skybox::Render()
{
	gD3DContext->VSSetShader(vertexShader, nullptr, 0);
	gD3DContext->PSSetShader(pixelShader, nullptr, 0);
	gD3DContext->OMSetBlendState(blendState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(depthStencilState, 0);
	gD3DContext->RSSetState(rasterizerState);
	gD3DContext->PSSetSamplers(0, 1, &samplerState);

	model->Render();
}