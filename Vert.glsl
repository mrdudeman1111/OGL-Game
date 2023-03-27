#version 440 core

#pragma shader_stage(vertex)

layout(std140, binding = 0) uniform CameraBuffer
{
    mat4 World;
    mat4 View;
    mat4 Proj;
} CBO;

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNorm;
layout(location = 2) in vec3 vCoord;

layout(location = 0) out vec3 OutvCoord;

const vec3 LightDir = normalize(vec3(0.f, 5.f, 5.f));

void main()
{
    gl_Position = CBO.Proj * CBO.View * CBO.World * vec4(vPos, 1.0);

    vec3 WorldNorm = normalize(mat3(CBO.World)*vNorm);
    float Intensity = max(dot(WorldNorm, LightDir), 0.0);
    OutvCoord = vCoord;
}