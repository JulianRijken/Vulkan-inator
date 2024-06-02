#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 viewProjection;
    vec4 viewPosition;
    int renderMode;
} ubo;

layout(push_constant) uniform constants
{
    mat4 model;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 outWorldPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outTangent;
layout(location = 3) out vec2 outUV;
layout(location = 4) out float outSign;

void main()
{
    outWorldPosition = vec3(push.model * vec4(inPosition, 1.0));
    outNormal =  normalize(mat3(push.model) * inNormal);
    outTangent = normalize(mat3(push.model) * inTangent.xyz);
    outUV = inUV;
    gl_Position =  ubo.viewProjection * vec4(outWorldPosition, 1.0);
    outSign = inTangent.a;
}