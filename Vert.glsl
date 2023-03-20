#version 440 core

#pragma shader_stage(vertex)

layout(std140, binding = 0) uniform CameraBuffer
{
    mat4 World;
    mat4 View;
    mat4 Proj;
} CBO;

layout(location = 0) in vec3 vPos;
// layout(location = 1) in vec4 vCol;

// layout(location = 0) out vec4 OutColor;

void main()
{
    gl_Position = CBO.Proj * CBO.View * CBO.World * vec4(vPos, 1.0);
    // OutColor = vCol;
}