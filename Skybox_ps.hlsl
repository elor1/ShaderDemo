#include "Common.hlsli"

TextureCube CubeMap : register(t0);
SamplerState TexSampler : register(s0);

float4 main(SkyboxPixelShaderInput input) : SV_Target{
	return CubeMap.Sample(TexSampler, input.position);
}