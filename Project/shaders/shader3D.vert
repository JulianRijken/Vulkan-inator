#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform constants
{
    mat4 model;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 outFragColor;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outFragTexCoord;
layout(location = 3) out vec4 outTangent;

void main()
{
    gl_Position = ubo.proj * ubo.view * push.model * vec4(inPosition, 1.0);

    mat4 invModel = inverse(push.model);
    mat3 modelRotationMatrix = mat3(transpose(invModel));

    // outFragColor = inColor;
    outFragColor = vec3(1,1,1);
    outNormal = modelRotationMatrix * inNormal;
    outFragTexCoord = inTexCoord;
}
