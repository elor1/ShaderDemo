#include "Common.hlsli"

LightingPixelShaderInput main(BasicVertex modelVertex)
{
    LightingPixelShaderInput output;

    float4 modelPosition = float4(modelVertex.position, 1); 

    // Multiply by the world matrix passed from C++ to transform the model vertex position into world space.
    float4 worldPosition     = mul(gWorldMatrix,      modelPosition);
    float4 viewPosition      = mul(gViewMatrix,       worldPosition);
    output.projectedPosition = mul(gProjectionMatrix, viewPosition);

    // Also transform model normals into world space using world matrix - lighting will be calculated in world space
    float4 modelNormal = float4(modelVertex.normal, 0);      
    output.worldNormal = mul(gWorldMatrix, modelNormal).xyz;
                                                             
    output.worldPosition = worldPosition.xyz; // Also pass world position to pixel shader for lighting

    // Pass texture coordinates (UVs) on to the pixel shader
    output.uv = modelVertex.uv;

    return output;
}
