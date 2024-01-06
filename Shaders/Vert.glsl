#version 440

uniform CameraBuffer
{
  mat4 World;
  mat4 View;
  mat4 Proj;
} Camera;

in vec3 inPos;
in vec3 inNorm;
in vec2 inCoord;

out vec2 outCoord;

void main()
{
  gl_Position = Camera.Proj * Camera.View * Camera.World * vec4(inPos, 1.f);

  outCoord = inCoord;
}

