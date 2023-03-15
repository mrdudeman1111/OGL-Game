#version 440
#pragma shader_stage(fragment)

layout(location=0) in vec4 Color;

layout(location=0) out vec4 OutColor;

void main()
{
    OutColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
