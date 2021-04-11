#include "Common.hlsli"

LightingPixelShaderInput main(BasicVertex modelVertex)
{
	LightingPixelShaderInput output;
	const float WIGGLE_MULTIPLIER = 10.0f;

	float4 modelPosition = float4(modelVertex.position, 1);

	// Multiply by the world matrix passed from C++ to transform the model vertex position into world space. 
	float4 worldPosition = mul(gWorldMatrix, modelPosition);

	float4 modelNormal = float4(modelVertex.normal, 0);
	float4 worldNormal = mul(gWorldMatrix, modelNormal);
	worldNormal = normalize(worldNormal);

	worldPosition.x += sin(modelPosition.y + (gTime * WIGGLE_MULTIPLIER)) * 0.1f;
	worldPosition.y += sin(modelPosition.x + (gTime * WIGGLE_MULTIPLIER)) * 0.1f;
	worldPosition += worldNormal * (sin(gTime * WIGGLE_MULTIPLIER) + 1.0f) * 0.1f;

	float4 viewPosition = mul(gViewMatrix, worldPosition);
	output.projectedPosition = mul(gProjectionMatrix, viewPosition);

	output.worldNormal = worldNormal.xyz; 
	output.worldPosition = worldPosition.xyz; // Also pass world position to pixel shader for lighting

	// Pass texture coordinates (UVs) on to the pixel shader
	output.uv = modelVertex.uv;

	return output;
}
