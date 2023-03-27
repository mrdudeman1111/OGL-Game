#version 440 core

#pragma shader_stage(fragment)

layout(location=0) in vec3 vCoord;

layout(location=0) out vec4 OutColor;

uniform sampler2D MeshTexture;

void main()
{
    OutColor = texture(MeshTexture, vec2(vCoord));
}