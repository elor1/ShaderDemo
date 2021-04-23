#include "Common.hlsli"

// Just returns a fixed outline colour
float4 main(SimplePixelShaderInput input) : SV_Target
{
	return float4(0.0f, 0.0f, 0.0f, 1.0f); // Always use 1.0f for alpha - no alpha blending in this lab
}