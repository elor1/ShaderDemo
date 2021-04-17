#pragma once
#include "CVector3.h"
#include "Model.h"
#include "SceneObject.h"

class Light : public SceneObject
{
public:
	Light(Model* Model, Texture* Texture, ID3D11VertexShader* VertexShader, ID3D11PixelShader* PixelShader,
		ID3D11BlendState* BlendState, ID3D11RasterizerState* RasterizerState,
		ID3D11DepthStencilState* DepthStencilState, ID3D11SamplerState* SamplerState, float Strength, CVector3 Colour);
	CVector3 Colour();
	float Strength();
	void SetColour(CVector3 Colour);
	void SetStrength(float Strength);

private:
	CVector3 colour;
	float    strength;
};

