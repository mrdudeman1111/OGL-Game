#version 330

layout(location=0) out vec3 GPos;
layout(location=1) out vec3 GNorm;
layout(location=2) out vec4 GAlbedo;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Norms;

uniform sampler2D MeshTexture;

void main()
{
    GPos = FragPos;
    GNorm = normalize(Norms);
    GAlbedo = texture(MeshTexture, TexCoord);
}