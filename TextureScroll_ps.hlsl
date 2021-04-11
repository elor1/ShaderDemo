#include "Common.hlsli"

Texture2D DiffuseSpecularMap : register(t0);
SamplerState TexSampler      : register(s0);

float4 main(LightingPixelShaderInput input) : SV_Target
{
// Normal might have been scaled by model scaling or interpolation so renormalise
input.worldNormal = normalize(input.worldNormal);
	
// Direction from pixel to camera
float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);

	
//// Calculate lighting ////
//Light 1
// Direction and distance from pixel to light
float3 light1Direction = normalize(gLight1Position - input.worldPosition);
float3 light1Dist = length(gLight1Position - input.worldPosition);

float3 diffuseLight1 = gLight1Colour * max(dot(input.worldNormal, light1Direction), 0) / light1Dist;
float3 halfway = normalize(light1Direction + cameraDirection);
float3 specularLight1 = diffuseLight1 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower); // Multiplying by diffuseLight instead of light colour - my own personal preference

//Light 2
float3 light2Vector = gLight2Position - input.worldPosition;
float  light2Distance = length(light2Vector);
float3 light2Direction = light2Vector / light2Distance;
float3 diffuseLight2 = 0;
float3 specularLight2 = 0;
if (dot(gLight2Facing, -light2Direction) > gLight2CosHalfAngle)
{
	diffuseLight2 = gLight2Colour * max(dot(input.worldNormal, light2Direction), 0) / light2Distance;
	halfway = normalize(light2Direction + cameraDirection);
	specularLight2 = diffuseLight2 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);
}

//Sum the effect of the lights
float3 diffuseLight = gAmbientColour + diffuseLight1 + diffuseLight2;
float3 specularLight = specularLight1 + specularLight2;

	
// Scroll texture
float uCoord = input.uv.x + (0.1f * gTime);
float vCoord = input.uv.y + (0.1f * gTime);
float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, float2(uCoord, vCoord));

//Add blue tint
float4 tintColour = { 0.0f, 0.0f, 0.8f, 0.0f };

// Combine texture with tint colour
textureColour += tintColour;
saturate(textureColour); ///Make sure values are clamped between 0-1
	
float3 diffuseMaterialColour = textureColour.rgb; // Diffuse material colour in texture RGB (base colour of model)
float specularMaterialColour = textureColour.a;   // Specular material colour in texture A (shininess of the surface)

// Combine lighting with texture colours
float3 finalColour = diffuseLight * diffuseMaterialColour + specularLight * specularMaterialColour;

return float4(finalColour, 1.0f);
}