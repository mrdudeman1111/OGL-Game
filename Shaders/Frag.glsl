#version 440

in vec2 inCoord;

out vec4 outColor;

uniform sampler2D MeshTexture;

void main()
{
  outColor = texture(MeshTexture, inCoord);
}

