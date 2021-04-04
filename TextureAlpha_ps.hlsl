//--------------------------------------------------------------------------------------
// Texture Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader simply samples a diffuse texture map with alpha

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
Texture2D    DiffuseSpecularMap : register(t0); // A diffuse map is the main texture for a model. It is the base colour of
										// the material in question. You may see the word "albedo map" also used for
										// this texture but there is a slight difference, covered in the MComp.
										// The t0 indicates this textuere is in slot 0. Each shader has a fixed number
										// of slots for textures. The C++ code must load the texture into the this slot

SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic
										// These are prepared in the C++ code and attached to a GPU sampler slot.
										// The s0 means use slot 0. There are a fixed number of slots for samplers.
										// Texture slots and sampler slots are independent from each other, i.e. You 
										// don't have to use the same number for both texture slot and sampler slot.


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples diffuse texture map (with alpha)
float4 main(LightingPixelShaderInput input) : SV_Target
{
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

float3 light2Direction = normalize(gLight2Position - input.worldPosition);
float3 light2Dist = length(gLight2Position - input.worldPosition);
float3 diffuseLight2 = gLight2Colour * max(dot(input.worldNormal, light2Direction), 0) / light2Dist;
halfway = normalize(light2Direction + cameraDirection);
float3 specularLight2 = diffuseLight2 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);


// Sum the effect of the lights - add the ambient at this stage rather than for each light (or we will get too much ambient)
float3 diffuseLight = gAmbientColour + diffuseLight1 + diffuseLight2;
float3 specularLight = specularLight1 + specularLight2;


////////////////////
// Combine lighting and textures

// Sample diffuse material and specular material colour for this pixel from a texture using a given sampler that you set up in the C++ code
float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv);
if (textureColour.a < 0.5f)
{
	discard;
}
float3 diffuseMaterialColour = textureColour.rgb; // Diffuse material colour in texture RGB (base colour of model)
float specularMaterialColour = textureColour.a;   // Specular material colour in texture A (shininess of the surface)

// Combine lighting with texture colours
float3 finalColour = diffuseLight * diffuseMaterialColour + specularLight * specularMaterialColour;

return float4(finalColour, 1.0f); // Always use 1.0f for output alpha - no alpha blending in this lab
}