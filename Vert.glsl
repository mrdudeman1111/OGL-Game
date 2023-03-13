#version 440
#pragma shader_stage(vertex)

layout(location=0) out vec3 OutColor;

void main()
{
    gl_Position = vec4(0,0,0,0);
    OutColor = vec3(1,1,1);
}