#pragma once

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

static const uint MAX_LIGHTS = 8;


struct Material
{
    float4 color;
    float2 tiling;
    float2 offset;
    float shininess;
    
};

struct Light
{
    float3 position;
    uint type;
    float3 direction;
    float range;
    float3 color;
    float intensity;
    float3 ambient;
    float innerCutoff;
    float outerCutoff;
    float3 padding;
    
};

//in lightuniform Light light[8] turns into lights[MAX_LIGHTS]
// add uint numLights
// add float3 ambient
// add to frag lightUniform
//normalize V
//set diffuse albedo
//for (uint i = 0; i < numLights; i++){
//flat3 diffuse = 0;
//float specular = 0;
//calculateLighting(lights[i], material, input.worldPosition);
//}
///output.color = float4(albedo.rgb * diffuse + specular, albedo.a);


float3 getLightVector(Light light, float3 position, out float attenuation)
{
    float3 L;
    attenuation = 1.0f;
    
    switch (light.type)
    {
        case LIGHT_DIRECTIONAL:
            L = normalize(-light.direction);
            break;
        case LIGHT_POINT:
            L = normalize(light.position - position);
            break;
        case LIGHT_SPOT:
            L = normalize(light.position - position);
            attenuation = smoothstep(light.outerCutoff, light.innerCutoff, dot(-L, light.direction));
            break;
    }
    
    return L;

    }

