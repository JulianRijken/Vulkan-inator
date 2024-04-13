#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
} vp;

layout(push_constant) uniform constants
{
    mat4 model;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 outFragColor;
layout(location = 1) out vec3 outNormal;


void main()
{
    gl_Position = vp.proj * vp.view * push.model * vec4(inPosition, 1.0);

    mat4 invModel = inverse(push.model);
    mat3 modelRotationMatrix = mat3(transpose(invModel));

    outFragColor = inColor;
    outNormal = modelRotationMatrix * inNormal;
}
