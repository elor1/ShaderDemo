//--------------------------------------------------------------------------------------
// Light Model Vertex Shader
//--------------------------------------------------------------------------------------
// Basic matrix transformations only

#include "Common.hlsli"

SimplePixelShaderInput main(BasicVertex modelVertex)
{
    SimplePixelShaderInput output;

    // Input position is x,y,z only
    float4 modelPosition = float4(modelVertex.position, 1); 

    // Multiply by the world matrix passed from C++ to transform the model vertex position into world space.
    float4 worldPosition     = mul(gWorldMatrix,      modelPosition);
    float4 viewPosition      = mul(gViewMatrix,       worldPosition);
    output.projectedPosition = mul(gProjectionMatrix, viewPosition);

    // Pass texture coordinates (UVs) on to the pixel shader
    output.uv = modelVertex.uv;

    return output; // Output data sent down the pipeline (to the pixel shader)
}
