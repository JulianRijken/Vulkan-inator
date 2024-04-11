#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
} vp;

layout(push_constant) uniform constants
{
    mat4 model;
} push;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;


void main()
{
    gl_Position = vp.proj * push.model * vec4(inPosition,0,1);
    fragColor = inColor;
}
