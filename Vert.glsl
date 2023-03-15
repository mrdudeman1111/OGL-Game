#version 440
#pragma shader_stage(vertex)

layout(binding=0) uniform CameraBuffer
{
    mat4 World;
    mat4 View;
    mat4 Projection;
} CBO;

layout(location=0) in vec3 VertPos;
// layout(location=1) in vec4 VertCol;
// layout(location=2) in vec3 VertNorm;

layout(location=0) out vec3 OutColor;

void main()
{
    gl_Position = CBO.Projection * CBO.View * CBO.World * VertPos;
    OutColor = VertCol;
}