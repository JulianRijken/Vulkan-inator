#version 450
#include "PBR.glsl"

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 viewProjection;
    vec4 viewPosition;
} ubo;

layout(set = 1,binding = 0) uniform sampler2D colorSample;
layout(set = 1,binding = 1) uniform sampler2D normalSample;
layout(set = 1,binding = 2) uniform sampler2D metallicSample;
layout(set = 1,binding = 3) uniform sampler2D roughnessSample;


layout(location = 0) in vec3 inWorldPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec4 outColor;

const vec3 AMBIENT_COLOR = vec3(0.1,0.1,0.1);
const vec3 LIGHT_DIRECTION = normalize(vec3(0,-1,-1));


struct Light
{
    vec3 position;
    vec3 color;
};


void main()
{
    vec3 N = calculateNormal(normalSample, inNormal, inTangent.xyz, inUV);
    vec3 V = normalize(ubo.viewPosition.xyz - inWorldPosition);
    vec3 R = reflect(-V, N);
    float metallic = texture(metallicSample, inUV).r;
    float roughness = texture(roughnessSample, inUV).r;

    vec3 baseColor = texture(colorSample, inUV).rgb;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor, metallic);


    const int numLights = 2;
    Light lights[numLights];
    lights[0].position = vec3(10.0, 0.0, 10.0);
    lights[0].color = vec3(1.0, 1.0, 1.0);

    lights[1].position = vec3(-100.0, 0.0, 0.0);
    lights[1].color = vec3(1.0, 1.0, 0.0);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lights.length(); i++)
    {
        vec3 L = normalize(lights[i].position - inWorldPosition);
        Lo += specularContribution(L, V, N, F0, metallic, roughness, inUV, baseColor);
    }

    vec2 brdf = (0.08 * vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 reflection = vec3(0.3);
    // Diffuse based on irradiance
    vec3 diffuse = 0.7 * baseColor;

    vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

    // Specular reflectance
    vec3 specular = reflection * (F * brdf.x + brdf.y);

    // Ambient part
    vec3 kD = 1.0 - F;
    kD *= 1.0 - metallic;
    vec3 ambient = (kD * diffuse + specular);

    vec3 color = ambient + Lo;

    outColor = vec4(color, 1.0);


    // outColor = texture(colorSample, inUV);
    // outColor = texture(normalSample, inUV);
    // outColor = texture(metallicSample, inUV);
    // outColor = texture(roughnessSample, inUV);

    // outColor = vec4(inNormal,1.0f);
    //outColor = vec4(inTangent,1.0f);
}




