//--------------------------------------------------------------------------------------
// Basic Transformation and Colour Vertex Shader
//--------------------------------------------------------------------------------------
// Shaders - we won't look at shaders until later in the module, but they are needed to render anything

#include "Common.hlsli" // Shaders can also use include files - note the extension



//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Vertex shader gets vertices from the mesh one at a time. It transforms their positions
// from 3D into 2D (see lectures) and passes that position down the pipeline so pixels can
// be rendered. 
SimplePixelShaderInput main(BasicVertex modelVertex)
{
	SimplePixelShaderInput output; // This is the data the pixel shader requires from this vertex shader
	const float WIGGLE_MULTIPLIER = 10.0f;

	// Input position is x,y,z only - need a 4th element to multiply by a 4x4 matrix. Use 1 for a point (0 for a vector) - recall lectures
	float4 modelPosition = float4(modelVertex.position, 1);

	// Multiply by the world matrix passed from C++ to transform the model vertex position into world space. 
	float4 worldPosition = mul(gWorldMatrix, modelPosition);

	float4 modelNormal = float4(modelVertex.normal, 0);
	float4 worldNormal = mul(gWorldMatrix, modelNormal);
	worldNormal = normalize(worldNormal);

	worldPosition.x += sin(modelPosition.y + (gTime * WIGGLE_MULTIPLIER)) * 0.1f;
	worldPosition.y += sin(modelPosition.x + (gTime * WIGGLE_MULTIPLIER)) * 0.1f;
	worldPosition += worldNormal * (sin(gTime * WIGGLE_MULTIPLIER) + 1.0f) * 0.1f;

	// In a similar way use the view matrix to transform the vertex from world space into view space (camera's point of view)
	// and then use the projection matrix to transform the vertex to 2D projection space
	float4 viewPosition = mul(gViewMatrix, worldPosition); // Read the comments, ah, but if you're reading this then you already knew that...
	output.projectedPosition = mul(gProjectionMatrix, viewPosition);

	// Temporary lines, replace each with the full lighting code as you do the lab exercise
	/*float3 attenuatedColour = lightColour / distance(lightPosition, worldPosition);
	output.diffuseLight = attenuatedColour * max(dot(worldNormal, lightPosition - worldPosition), 0) + ambientColour;
	output.specularLight = attenuatedColour * pow(max(dot(worldNormal, normalize((lightPosition - worldPosition) + (viewPosition - worldPosition))), 0), specularPower);*/

	// Pass texture coordinates (UVs) on to the pixel shader, the vertex shader doesn't need them
	output.uv = modelVertex.uv;

	return output; // Ouput data sent down the pipeline (to the pixel shader)
}
