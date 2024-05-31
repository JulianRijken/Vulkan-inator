#version 450
#include "PBR.glsl"

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 viewProjection;
    vec4 viewPosition;
    int renderMode;
} ubo;


layout(set = 1, binding = 0) uniform sampler2D colorSampler;
layout(set = 1, binding = 1) uniform sampler2D normalSampler;
layout(set = 1, binding = 2) uniform sampler2D metallicSampler;
layout(set = 1, binding = 3) uniform sampler2D roughnessSampler;
layout(set = 1, binding = 4) uniform sampler2D ambientOcclusionSampler;

layout(location = 0) in vec3 inWorldPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec4 outColor;

struct Light
{
    vec3 position;
    vec3 color;
};


vec3 GetColorPBR()
{
    float metallic = texture(metallicSampler, inUV).r;
    float roughness = texture(roughnessSampler, inUV).r;
    vec3 albedo = texture(colorSampler, inUV).rgb;

    // Calculate normal
    vec3 normal = normalize(inNormal);
    vec3 tangent = normalize(inTangent.xyz);
    vec3 bitangent = normalize(cross(normal, tangent));
    mat3 tbn = mat3(tangent, bitangent, normal);
    vec3 sampledNormal = normalize(tbn * (texture(normalSampler, inUV).xyz * 2.0 - 1.0));


    vec3 worldToView = normalize(ubo.viewPosition.xyz - inWorldPosition);

    // Reflection is the reflection from view to world based on the normal
    vec3 reflection = reflect(-worldToView, sampledNormal);

    // Fresnel Mix
    vec3 F0 = mix(vec3(0.04), albedo, metallic);


    const int numLights = 4;
    Light lights[numLights];
    lights[0].position = vec3(10.0, 0.0, 10.0);
    lights[0].color = vec3(1.0, 1.0, 1.0);
    lights[1].position = vec3(-100.0, 0.0, 0.0);
    lights[1].color = vec3(1.0, 1.0, 0.0);
    lights[2].position = vec3(50.0, 50.0, 0.0);
    lights[2].color = vec3(0.0, 1.0, 1.0);

    vec3 finalColor;

    for(int i = 0; i < numLights; i++)
    {
        vec3 worldToLight = normalize(lights[i].position - inWorldPosition);
    }

    return finalColor;


    // vec3 N = CalculateNormal(normalSample, inNormal, inTangent.xyz, inUV);
    // vec3 V = normalize(ubo.viewPosition.xyz - inWorldPosition);
    // vec3 R = reflect(-V, N);
    // float metallic = texture(metallicSample, inUV).r;
    // float roughness = texture(roughnessSample, inUV).r;

    // vec3 albedo = texture(colorSample, inUV).rgb * texture(ambientOcclusionSample, inUV).rgb;

    // vec3 F0 = vec3(0.04);
    // F0 = mix(F0, albedo, metallic);


    // //Hard Define lights for now
    // const int numLights = 4;
    // Light lights[numLights];
    // lights[0].position = vec3(10.0, 0.0, 10.0);
    // lights[0].color = vec3(1.0, 1.0, 1.0);
    // lights[1].position = vec3(-100.0, 0.0, 0.0);
    // lights[1].color = vec3(1.0, 1.0, 0.0);
    // lights[2].position = vec3(50.0, 50.0, 0.0);
    // lights[2].color = vec3(0.0, 1.0, 1.0);
    // vec3 Lo = vec3(0.0);
    // for(int i = 0; i < lights.length(); i++)
    // {
    //     vec3 L = normalize(lights[i].position - inWorldPosition);
    //     Lo += SpecularContribution(L, V, N, F0, metallic, roughness, inUV, albedo);
    // }


    // vec2 brdf = (0.08 * vec2(max(dot(N, V), 0.0), roughness)).rg;
    // vec3 reflection = vec3(0.3);

    // vec3 diffuse = 0.7 * albedo;

    // vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

    // // Specular reflectance
    // vec3 specular = reflection * (F * brdf.x + brdf.y);

    // // Ambient part
    // vec3 kD = 1.0 - F;
    // kD *= 1.0 - metallic;

    // vec3 ambient = 0.01 +  (kD * diffuse + specular);


    // vec3 color = ambient + Lo;

    // return color;
    // lights[3].position = vec3(0.0, 1.0, 10.0);
    // lights[3].color = vec3(1.0, 1.0, 1.0);

}




void main()
{
    if(ubo.renderMode == 0)
    {
        outColor = vec4(GetColorPBR(),1.0f);
        return;
    }
    else if(ubo.renderMode == 1)
    {
        outColor = texture(colorSampler, inUV);
        return;
    }
    else if(ubo.renderMode == 2)
    {
        outColor = texture(normalSampler, inUV);
        return;
    }
    else if(ubo.renderMode == 3)
    {
        outColor = texture(metallicSampler, inUV);
        return;
    }
    else if(ubo.renderMode == 4)
    {
        outColor = texture(roughnessSampler, inUV);
        return;
    }
    else if(ubo.renderMode == 5)
    {
        // vec3 N = calculateNormal(normalSample, inNormal, inTangent.xyz, inUV);
        outColor = vec4(inNormal ,1.0f);
        return;
    }
    else if(ubo.renderMode == 6)
    {
        outColor = vec4(inTangent,1.0f);
        return;
    }
}



