#include "Common.hlsli"

Texture2D DiffuseSpecularMap : register(t0); 
Texture2D NormalMap          : register(t1); 
SamplerState TexSampler : register(s0);

float4 main(NormalMappingPixelShaderInput input) : SV_Target
{
	// The normals for each pixel are interpolated from the vertex normals/tangents
	//This means they will not be length 1, so they need to be renormalised
	float3 modelNormal = normalize(input.modelNormal);
	float3 modelTangent = normalize(input.modelTangent);

	// Calculate bi-tangent to complete the three axes of tangent space
	// Then create the inverse tangent matrix to convert from tangent space into model space. 
	float3 modelBiTangent = cross(modelNormal, modelTangent) * 15;
	float3x3 invTangentMatrix = float3x3(modelTangent, modelBiTangent, modelNormal);

	// Get the texture normal from the normal map
	float3 textureNormal = 2.0f * NormalMap.Sample(TexSampler, input.uv).rgb - 1.0f; // Scale from 0->1 to -1->1

	// Convert the texture normal into model space using the inverse tangent matrix, and then convert into world space using the world
	// matrix. Normalise, because of the effects of texture filtering and in case the world matrix contains scaling
	float3 worldNormal = normalize(mul((float3x3)gWorldMatrix, mul(textureNormal, invTangentMatrix)));

	
	//// Calculate lighting ////
	
	float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);

	// Light 1
	float3 light1Vector = gLight1Position - input.worldPosition;
	float  light1Distance = length(light1Vector);
	float3 light1Direction = light1Vector / light1Distance; // Quicker than normalising as we have length for attenuation
	float3 diffuseLight1 = gLight1Colour * max(dot(worldNormal, light1Direction), 0) / light1Distance;

	float3 halfway = normalize(light1Direction + cameraDirection);
	float3 specularLight1 = diffuseLight1 * pow(max(dot(worldNormal, halfway), 0), gSpecularPower);

	// Light 2
	float3 light2Vector = gLight2Position - input.worldPosition;
	float  light2Distance = length(light2Vector);
	float3 light2Direction = light2Vector / light2Distance;
	float3 diffuseLight2 = 0;
	float3 specularLight2 = 0;
	if (dot(gLight2Facing, -light2Direction) > gLight2CosHalfAngle)
	{
		diffuseLight2 = gLight2Colour * max(dot(worldNormal, light2Direction), 0) / light2Distance;
		halfway = normalize(light2Direction + cameraDirection);
		specularLight2 = diffuseLight2 * pow(max(dot(worldNormal, halfway), 0), gSpecularPower);
	}


	// Sample diffuse material colour for this pixel from a texture
	float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv);
	float3 diffuseMaterialColour = textureColour.rgb;
	float specularMaterialColour = textureColour.a;

	//Combine texture & lighting
	float3 finalColour = (gAmbientColour + diffuseLight1 + diffuseLight2) * diffuseMaterialColour +
						 (specularLight1 + specularLight2) * specularMaterialColour;
	
	return float4(finalColour, 1.0f);
}