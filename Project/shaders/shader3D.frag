#version 450


layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inFragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    const vec3 lightDirection = normalize(vec3(0,-1,-1));

    float observedArea = max(0.02f, dot(normalize(inNormal), -lightDirection));
    outColor = vec4(inFragColor * observedArea, 1.0) * texture(texSampler, inFragTexCoord);
}
