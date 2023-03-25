#version 440 core

#pragma shader_stage(fragment)

layout(location=0) in vec4 InColor;

layout(location=0) out vec4 OutColor;

void main()
{
    OutColor = vec4(1.0, 1.0, 1.0, 1.0);
}