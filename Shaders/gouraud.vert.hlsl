#include "Lighting.hlsli"

cbuffer VertexUniforms : register(b0, space1)
{
    float4x4 mvp;
    float4x4 model;
    float3 cameraPosition;
    float padding1;
}

cbuffer LightUniforms : register(b1, space1)
{
    Light lights[MAX_LIGHTS];
    uint numLights;
    float3 ambient;
}

cbuffer MaterialUniforms : register(b2, space1)
{
    Material material;
}

struct Input
{
    float3 position : TEXCOORD0;
    float2 texcoord : TEXCOORD1;
    float3 normal : TEXCOORD2;
};

struct Output
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float3 diffuse : TEXCOORD1;
    float3 specular : TEXCOORD2;
};

Output main(Input input)
{
    Output output;

    output.position = mul(mvp, float4(input.position, 1.0f));
    output.texcoord = input.texcoord * material.tiling + material.offset;

    float3 worldPosition = mul(model, float4(input.position, 1.0f)).xyz;
    float3 N = normalize(mul((float3x3) model, input.normal));
    float3 V = normalize(cameraPosition - worldPosition);

    float3 diffuse = ambient;
    float3 specular = float3(0.0f, 0.0f, 0.0f);

    for (uint i = 0; i < numLights; i++)
    {
        float attenuation;
        float3 L = getLightVector(lights[i], worldPosition, attenuation);
        float3 radiance = lights[i].color * lights[i].intensity * attenuation;

        diffuse += calculateDiffuse(radiance, N, L);
        specular += calculateSpecular(radiance, N, L, V, material);
    }

    output.diffuse = diffuse;
    output.specular = specular;

    return output;
}