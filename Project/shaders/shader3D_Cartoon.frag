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
layout(location = 4) in float inSign;

layout(location = 0) out vec4 pixel;

const vec3 g_LightDirection = normalize(vec3(-0.5,-1,0.5));


void main()
{
    float time = ubo.renderMode / 400.0f;
    time *= 0.01;
    // float metallic = texture(metallicSampler, inUV).r;
    // float roughness = texture(roughnessSampler, inUV).r;


    vec2 displacedUV = (texture(normalSampler, inUV * 0.8f).rr * 2 - 1) * 0.4f + inUV;

    vec2 uv1 = displacedUV;
    uv1.x += time;
    uv1.y += time * 0.3f;

    vec2 uv2 = displacedUV;
    uv2.y += time * 0.6;
    uv2.x += time * 0.5f;

    vec2 uv3 = displacedUV;
    uv3.y += -time * 0.4;
    uv3.x += -time * 0.3f;

    vec3 water1 = texture(colorSampler, uv1 * 16).rgb;
    vec3 water2 = texture(colorSampler, uv2 * 6).rgb;
    vec3 water3 = texture(colorSampler, uv3 * 8).rgb;

    float water = min(vec3(1,1,1),water1 + water2 + water3).r;
    vec3 color = mix(vec3(0.05f, 0.76f, 0.89f),vec3(0.34f, 0.91f, 0.91f),water);


    // vec3 normalMap = texture(normalSampler, inUV).xyz * 2.0 - 1.0;

    // // Calculate normal
    vec3 normal = inNormal;
    // vec3 tangent = inTangent;
    // vec3 bitangent = normalize(cross(normal, tangent)) * inSign;
    // mat3 tbn = mat3(tangent, bitangent, normal);
    // vec3 sampledNormal = normalize(tbn * normalMap);

    // // V
    // vec3 worldToView = normalize(ubo.viewPosition.xyz - inWorldPosition);

    // // Fresnel Mix
    // vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // //Hard Define lights for now
    // const int numLights = 2;
    // Light lights[numLights];
    // lights[0].position = vec3(10.0, 0.0, 10.0);
    // lights[0].color = vec3(1.0, 1.0, 1.0);
    // lights[1].position = vec3(-100.0, 0.0, 0.0);
    // lights[1].color = vec3(1.0, 1.0, 0.0);

    // vec3 Lo = vec3(0.0);
    // for(int i = 0; i < lights.length(); i++)
    // {
    //     vec3 L = normalize(lights[i].position - inWorldPosition);

    //      Lo += SpecularContribution(L, worldToView, sampledNormal, F0, metallic, roughness, inUV, albedo);
    // }

    // vec2 brdf = (0.08 * vec2(max(dot(sampledNormal, worldToView), 0.0), roughness)).rg;
    // vec3 reflection = vec3(0.3);

    // vec3 diffuse = 0.7 * albedo;

    // vec3 F = F_SchlickR(max(dot(sampledNormal, worldToView), 0.0), F0, roughness);

    // // Specular reflectance
    // vec3 specular = reflection * (F * brdf.x + brdf.y);

    float observedArea = max(0.7f, dot(normal, -g_LightDirection));

    // // Ambient part
    // vec3 kD = 1.0 - F;
    // kD *= 1.0 - metallic;

    // vec3 ambient = 0.01 +  (kD * diffuse + specular);

    // vec3 color = ambient + Lo;

    // return color * observedArea;


    pixel = vec4(color * observedArea,1);
}



