#include "Common.hlsli"

NormalMappingPixelShaderInput main(TangentVertex modelVertex)
{
	NormalMappingPixelShaderInput output;
	
	float4 modelPosition = float4(modelVertex.position, 1);

	// Multiply by the world matrix passed from C++ to transform the model vertex position into world space.
	float4 worldPosition = mul(gWorldMatrix, modelPosition);
	float4 viewPosition = mul(gViewMatrix, worldPosition);
	output.projectedPosition = mul(gProjectionMatrix, viewPosition);

	output.worldPosition = worldPosition.xyz; // Also pass world position to pixel shader for lighting

	// Unlike the position, send the model's normal and tangent untransformed (in model space). The pixel shader will do the matrix work on normals
	output.modelNormal = modelVertex.normal;
	output.modelTangent = modelVertex.tangent;

	// Pass texture coordinates (UVs) on to the pixel shader
	output.uv = modelVertex.uv;

	return output;
}
