#version 450
#include "PBR.glsl"

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 viewProjection;
    vec4 viewPosition;
} ubo;

layout(set = 1,binding = 0) uniform sampler2D baseColor;
layout(set = 1,binding = 1) uniform sampler2D normal;
layout(set = 1,binding = 2) uniform sampler2D metallic;
layout(set = 1,binding = 3) uniform sampler2D roughness;


layout(location = 0) in vec3 inWorldPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec4 outColor;

const vec3 g_AmbientColor = vec3(0.1,0.1,0.1);
const vec3 g_LightDirection = normalize(vec3(0,-1,-1));

void main()
{
    outColor = texture(baseColor, inUV);
}




