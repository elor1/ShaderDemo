//--------------------------------------------------------------------------------------
// Light Model Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader simply samples a diffuse texture map and tints with a fixed colour sent over from the CPU via a constant buffer

#include "Common.hlsli"

Texture2D    DiffuseMap : register(t0);                                 
SamplerState TexSampler : register(s0);

float4 main(SimplePixelShaderInput input) : SV_Target
{
    // Sample diffuse material colour for this pixel from a texture using a given sampler
    // Ignoring any alpha in the texture, just reading RGB
    float3 diffuseMapColour = DiffuseMap.Sample(TexSampler, input.uv).rgb;

    // Blend texture colour with fixed per-object colour
    float3 finalColour = gObjectColour * diffuseMapColour;

    return float4(finalColour, 1.0f); // Always use 1.0f for alpha - no alpha blending in this lab
}