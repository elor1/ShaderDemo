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
float4 textureColour = lerp(texture1Colour, texture2Colour, lerpValue);

// Normal might have been scaled by model scaling or interpolation so renormalise
input.worldNormal = normalize(input.worldNormal);

///////////////////////
// Calculate lighting

// Direction from pixel to camera
float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);

//// Light 1 ////

// Direction and distance from pixel to light
float3 light1Direction = normalize(gLight1Position - input.worldPosition);
float3 light1Dist = length(gLight1Position - input.worldPosition);

// Equations from lighting lecture
float3 diffuseLight1 = gLight1Colour * max(dot(input.worldNormal, light1Direction), 0) / light1Dist;
float3 halfway = normalize(light1Direction + cameraDirection);
float3 specularLight1 = diffuseLight1 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower); // Multiplying by diffuseLight instead of light colour - my own personal preference


//// Light 2 ////

float3 light2Vector = gLight2Position - input.worldPosition;
float  light2Distance = length(light2Vector);
float3 light2Direction = light2Vector / light2Distance;
float3 diffuseLight2 = 0;
float3 specularLight2 = 0;
// Light 2
if (dot(gLight2Facing, -light2Direction) > cos(45))
{
	diffuseLight2 = gLight2Colour * max(dot(input.worldNormal, light2Direction), 0) / light2Distance;
	halfway = normalize(light2Direction + cameraDirection);
	specularLight2 = diffuseLight2 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);
}


// Sum the effect of the lights - add the ambient at this stage rather than for each light (or we will get too much ambient)
float3 diffuseLight = gAmbientColour + diffuseLight1 + diffuseLight2;
float3 specularLight = specularLight1 + specularLight2;


////////////////////
// Combine lighting and textures

// Sample diffuse material and specular material colour for this pixel from a texture using a given sampler that you set up in the C++ code
float3 diffuseMaterialColour = textureColour.rgb; // Diffuse material colour in texture RGB (base colour of model)
float specularMaterialColour = textureColour.a;   // Specular material colour in texture A (shininess of the surface)

// Combine lighting with texture colours
float3 finalColour = diffuseLight * diffuseMaterialColour + specularLight * specularMaterialColour;

return float4(finalColour, 1.0f); // Always use 1.0f for output alpha - no alpha blending in this lab

//return finalColour; // Always use 1.0f for output alpha - no alpha blending in this lab

	
}