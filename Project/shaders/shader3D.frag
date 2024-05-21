#version 450
#include "PBR.glsl"

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
} ubo;


layout(set = 1,binding = 0) uniform sampler2D baseColor;
layout(set = 1,binding = 1) uniform sampler2D metallic;
layout(set = 1,binding = 2) uniform sampler2D normal;
layout(set = 1,binding = 3) uniform sampler2D roughness;


layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;
layout(location = 2) in vec2 inFragTexCoord;

layout(location = 0) out vec4 outColor;

const vec3 g_AmbientColor = vec3(0.1,0.1,0.1);
const vec3 g_LightDirection = normalize(vec3(0,-1,-1));


void main()
{
    vec4 ambientColor = vec4(g_AmbientColor, 1.0f) * ubo.view;
    vec4 finalColor = ambientColor;

    float observedArea = max(0.02f, dot(normalize(inNormal), -g_LightDirection));
    outColor = vec4(inFragColor * observedArea, 1.0) * texture(baseColor, inFragTexCoord);

}




