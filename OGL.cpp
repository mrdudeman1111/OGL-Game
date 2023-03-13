#include <iostream>
#include <fstream>
#include <vector>

#include <OGL.h>

#define STBI_IMPLEMENTATION
#include <stb/stb_image.h>

std::string LoadFile(const char* ShaderPath)
{
    std::string pCode;

    std::ifstream FileStream(ShaderPath, std::ios::in);

    if(!FileStream.is_open())
    {
        throw std::runtime_error("Failed to Open File for Read/Write");
    }

    size_t FileSize = FileStream.tellg();
    FileStream.seekg(0);

    FileStream.read(pCode.data(), FileSize);

    return pCode;
}

GLuint LoadTexture(const char* TexPath)
{
    stbi_uc* ImageChar;
    int Width, Height, Channels;
    ImageChar = stbi_load(TexPath, &Width, &Height, &Channels, 4);

    GLuint GlTex;
    glGenTextures(1, &GlTex);
    glBindTexture(GL_TEXTURE_2D, GlTex);

    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageChar);

    stbi_image_free(ImageChar);
}

int main()
{
    if(glfwInit())
    {
        throw std::runtime_error("Failed to Initiate glfw");
    }

    glfwWindowHint(GLFW_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_VERSION_MINOR, 0);
    GLFWwindow* Window = glfwCreateWindow(1280, 720, "Opengl Sample", NULL, NULL);

    glfwMakeContextCurrent(Window);

    if(glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to Initiate glew");
    }

    glViewport(0, 0, 1280, 720);

    GLuint VertShader = glCreateShader(GL_VERTEX_SHADER);

    std::string VertCode = LoadFile("~/Repos/HIP/Vert.glsl");
    const char* pVertCode = VertCode.c_str();

    glShaderSource(VertShader, 1, &pVertCode, NULL);
    glCompileShader(VertShader);

    GLuint FragShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string FragCode = LoadFile("~/Repos/HIP/Frag.glsl");
    const char* pFragCode = FragCode.c_str();

    glShaderSource(FragShader, 1, &pFragCode, NULL);
    glCompileShader(FragShader);

    GLuint Program = glCreateProgram();
    glAttachShader(Program, VertShader);
    glAttachShader(Program, FragShader);

    glClearColor(0,0,0,1);

    glUseProgram(Program);

    GLuint FrameBuffers[3];

    glClearColor(0, 0, 0, 1);

    glGenFramebuffers(3, FrameBuffers);

    while(!glfwWindowShouldClose(Window))
    {
        glfwPollEvents();

        glfwSwapBuffers(Window);
    }

    glfwDestroyWindow(Window);

    return 0;
}
