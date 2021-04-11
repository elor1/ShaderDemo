#include "Common.hlsli"

Texture2D DiffuseSpecularMap : register(t0); 
SamplerState TexSampler      : register(s0); 

float4 main(LightingPixelShaderInput input) : SV_Target
{
    // Normal might have been scaled by model scaling or interpolation so renormalise
    input.worldNormal = normalize(input.worldNormal); 


	//// Calculate lighting ////
    
    // Direction from pixel to camera
    float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);

	//Light 1
	// Direction and distance from pixel to light
	float3 light1Direction = normalize(gLight1Position - input.worldPosition);
    float3 light1Dist = length(gLight1Position - input.worldPosition);
    
    float3 diffuseLight1 = gLight1Colour * max(dot(input.worldNormal, light1Direction), 0) / light1Dist; 
    float3 halfway = normalize(light1Direction + cameraDirection);
    float3 specularLight1 =  diffuseLight1 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);

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


	// Sum the effect of the lights - add the ambient at this stage rather than for each light
	float3 diffuseLight = gAmbientColour + diffuseLight1 + diffuseLight2;
	float3 specularLight = specularLight1 + specularLight2;

    // Sample diffuse material and specular material colour for this pixel from a texture
    float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv);
    float3 diffuseMaterialColour = textureColour.rgb; // Diffuse material colour in texture RGB (base colour of model)
    float specularMaterialColour = textureColour.a;   // Specular material colour in texture A (shininess of the surface)

    // Combine lighting with texture colours
    float3 finalColour = diffuseLight * diffuseMaterialColour + specularLight * specularMaterialColour;

    return float4(finalColour, 1.0f);
}