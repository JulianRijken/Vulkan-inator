#version 450
#include "PBR.glsl"

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 viewProjection;
    vec4 viewPosition;
    int renderMode;
} ubo;


layout(set = 1, binding = 0) uniform sampler2D colorSampler;
layout(set = 1, binding = 1) uniform sampler2D distortionSampler;
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

    // Credits to Mat for the idea of displacement
    vec2 displacedUV = (texture(distortionSampler, inUV * 0.8f).rr * 2 - 1) * 0.4f + inUV;

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
    float observedArea = max(0.7f, dot(inNormal, -g_LightDirection));

    pixel = vec4(color * observedArea,1);
}



