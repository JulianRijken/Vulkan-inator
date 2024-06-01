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
    vec3 worldPostition = vec3(push.model * vec4(inPosition, 1.0));
    float time = ubo.renderMode / 400.0f;

    float waveSine = (sin(time + inUV.x * 30) + 1) * 0.5;
    float waveCos = (cos(time * 0.2f+ inUV.y * 30) + 1) * 0.5;
    float waveCosSmall = (cos(time * 0.1f + inUV.y * 90 + inUV.x * 90 ) + 1) * 0.5;
    worldPostition.y += (waveSine * 2.5f + waveCos * 3.0f + waveCosSmall * 1.0f) * 0.8f;


    outWorldPosition = worldPostition;
    vec3 normal = normalize(vec3(waveSine,waveCos,waveCosSmall * 0.1f));

    outNormal = normal ;
    outTangent = normalize(mat3(push.model) * inTangent.xyz);
    outUV = inUV;



    gl_Position =  ubo.viewProjection * vec4(outWorldPosition, 1.0);



    outSign = inTangent.a;

}
