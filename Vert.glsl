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

layout(location = 0) out vec4 OutColor;

const vec3 LightDir = vec3(0.f, 1.f, 4.f);

void main()
{
    gl_Position = CBO.Proj * CBO.View * CBO.World * vec4(vPos, 1.0);
    OutColor = vec4(1.f, 1.f, 1.f, 1.f) * dot(vNorm, LightDir);
}