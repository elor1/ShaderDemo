#include "Light.h"

//Light::~Light()
//{
//	delete model;
//	model = nullptr;
//}
Light::Light(Model* Model, Texture* Texture, ID3D11VertexShader* VertexShader, ID3D11PixelShader* PixelShader,
	ID3D11BlendState* BlendState, ID3D11RasterizerState* RasterizerState, ID3D11DepthStencilState* DepthStencilState,
	ID3D11SamplerState* SamplerState, float Strength, CVector3 Colour) : SceneObject(Model, Texture, VertexShader, PixelShader, BlendState,
	                                                RasterizerState, DepthStencilState, SamplerState, false)
{
	strength = Strength;
	colour = Colour;
}

CVector3 Light::Colour()
{
	return colour;
}

float Light::Strength()
{
	return strength;
}

void Light::SetColour(CVector3 Colour)
{
	colour = Colour;
}

void Light::SetStrength(float Strength)
{
	strength = Strength;
}
