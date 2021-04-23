//--------------------------------------------------------------------------------------
// Vertex shader for cell shading
//--------------------------------------------------------------------------------------

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
Texture2D    DiffuseMap : register(t0); // Diffuse map only
Texture2D    CellMap    : register(t1); // CellMap is a 1D map that is used to limit the range of colours used in cell shading

SamplerState TexSampler       : register(s0); // Sampler for use on textures
SamplerState PointSampleClamp : register(s1); // No filtering of cell maps (otherwise the cell edges would be blurred)


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(LightingPixelShaderInput input) : SV_Target
{
	// Lighting equations
	input.worldNormal = normalize(input.worldNormal); // Normal might have been scaled by model scaling or interpolation so renormalise
	float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);

	//Light 1
	// Direction and distance from pixel to light
	float3 light1Direction = normalize(gLight1Position - input.worldPosition);
	float3 light1Dist = length(gLight1Position - input.worldPosition);

	float  diffuseLevel1 = max(dot(input.worldNormal, light1Direction), 0);
	float  cellDiffuseLevel1 = CellMap.Sample(PointSampleClamp, diffuseLevel1).r;
	float3 diffuseLight1 = gLight1Colour * cellDiffuseLevel1 / light1Dist;
	float3 halfway = normalize(light1Direction + cameraDirection);
	float3 specularLight1 = diffuseLight1 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);

	//Light 2
	float3 light2Vector = gLight2Position - input.worldPosition;
	float  light2Distance = length(light2Vector);
	float3 light2Direction = light2Vector / light2Distance;
	float3 diffuseLight2 = 0;
	float3 specularLight2 = 0;
	if (dot(gLight2Facing, -light2Direction) > gLight2CosHalfAngle)
	{
		float  diffuseLevel2 = max(dot(input.worldNormal, light2Direction), 0);
		float  cellDiffuseLevel2 = CellMap.Sample(PointSampleClamp, diffuseLevel2).r;
		diffuseLight2 = gLight2Colour * cellDiffuseLevel2 / light2Distance;
		halfway = normalize(light2Direction + cameraDirection);
		specularLight2 = diffuseLight2 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);
	}
	
	// Sum the effect of the lights - add the ambient at this stage rather than for each light
	float3 diffuseLight = gAmbientColour + diffuseLight1 + diffuseLight2;
	float3 specularLight = specularLight1 + specularLight2;

	// Sample diffuse material and specular material colour for this pixel from a texture
	float4 textureColour = DiffuseMap.Sample(TexSampler, input.uv);
	float3 diffuseMaterialColour = textureColour.rgb; // Diffuse material colour in texture RGB (base colour of model)
	float specularMaterialColour = textureColour.a;   // Specular material colour in texture A (shininess of the surface)

	// Combine lighting with texture colours
	float3 finalColour = diffuseLight * diffuseMaterialColour + specularLight * specularMaterialColour;

	return float4(finalColour, 1.0f);
}