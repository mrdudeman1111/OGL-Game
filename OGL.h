#include <GL/glew.h>
#include <GL/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

std::string LoadFile(const char* ShaderPath);

GLuint LoadTexture(const char* TexPath);
