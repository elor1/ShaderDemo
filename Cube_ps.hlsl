// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
#include "Common.hlsli"
Texture2D DiffuseSpecularMap1 : register(t0); // Textures here can contain a diffuse map (main colour) in their rgb channels and a specular map (shininess) in the a channel
Texture2D DiffuseSpecularMap2 : register(t1);
SamplerState TexSampler      : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic - this is the sampler used for the texture above

//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(LightingPixelShaderInput input) : SV_Target
{
float4 texture1Colour = DiffuseSpecularMap1.Sample(TexSampler, input.uv);
float4 texture2Colour = DiffuseSpecularMap2.Sample(TexSampler, input.uv);

float lerpValue = 0.5f * (sin(gTime)) + 0.5f;
	
//Combine texture colours
float4 finalColour = lerp(texture1Colour, texture2Colour, lerpValue);

return finalColour; // Always use 1.0f for output alpha - no alpha blending in this lab
}