#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

void main()
{
    const vec3 lightDirection = normalize(vec3(0,-1,-1));

    float observedArea = max(0.0f, dot(normalize(inNormal), -lightDirection));
    outColor = vec4(fragColor * observedArea, 1.0);
}
