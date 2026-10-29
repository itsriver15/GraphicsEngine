#include "Lighting.hlsli"

cbuffer Uniforms : register(b0, space1)
{
    float4x4 mvp;
    float4x4 model;
    float3 cameraPosition;
    float padding1;
};

cbuffer LightUniforms : register(b1, space1)
{
    Light light;
};

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
    output.texcoord = (input.texcoord * material.offset) + material.offset;
    output.position = mul(mvp, float4(input.position, 1.0f));
    
    float3 worldPosition = mul(model, float4(input.position, 1.0f)).xyz;
    
    //DIFFUSE
    float3 N = normalize(mul((float3x3) model, input.normal));
    float3 L;
    
    if (light.type == LIGHT_DIRECTIONAL)
    {
        L = normalize(-light.direction);

    }
    else if (light.type == LIGHT_POINT)
    {
        L = normalize((light.position - worldPosition));
    }
    else
    {
        L = float3(0.0f, 0.0f, 0.0f);
    }
    float3 radiance = light.color * light.intensity;
    
    float NdotL = saturate(dot(N, L));
    
    float3 diffuse = radiance * NdotL;
    
    output.diffuse = light.ambient + diffuse;
    
    //SPECULAR
    output.specular = 0;
    
    if (NdotL > 0)
    {
        float3 R = reflect(-L, N);
        float3 V = normalize(cameraPosition - worldPosition);
        float RdotV = saturate(dot(R, V));
        
        output.specular = radiance * pow(RdotV, material.shininess);

    }
  
    return output;
}