#include "Common.hlsli"

SkyboxPixelShaderInput main(SkyboxVertex modelVertex)
{
	SkyboxPixelShaderInput output;

	/*output.projectedPosition = mul(float4(modelVertex.position, 1.0f), gViewProjectionMatrix * gWorldMatrix).xyww;

	output.position = modelVertex.position;*/
	// Input position is x,y,z only
	float4 modelPosition = float4(modelVertex.position, 1);

	// Multiply by the world matrix passed from C++ to transform the model vertex position into world space.
	float4 worldPosition = mul(gWorldMatrix, modelPosition);
	float4 viewPosition = mul(gViewMatrix, worldPosition);
	output.projectedPosition = mul(gProjectionMatrix, viewPosition).xyww;
	
	// Pass texture coordinates (UVs) on to the pixel shader
	output.position = modelVertex.position;

	return output;
}