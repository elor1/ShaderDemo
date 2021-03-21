// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
#include "Common.hlsli"
Texture2D DiffuseSpecularMap : register(t0); // Textures here can contain a diffuse map (main colour) in their rgb channels and a specular map (shininess) in the a channel
SamplerState TexSampler      : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic - this is the sampler used for the texture above


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(SimplePixelShaderInput input) : SV_Target
{
float uCoord = input.uv.x + (0.1f * gTime);
float vCoord = input.uv.y + (0.1f * gTime);
float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, float2(uCoord, vCoord));

float4 tintColour = { 0.0f, 0.0f, 0.8f, 1.0f };

// Combine texture with tint colour
float4 finalColour = textureColour + tintColour;

return finalColour; // Always use 1.0f for output alpha - no alpha blending in this lab
}