#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <stdio.h>
#include <chrono>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Vertex.h>

#include <OGL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

GLFWwindow *Window;

const char *LoadFile(const char *ShaderPath)
{
    std::ifstream FileStream(ShaderPath);

    FileStream.seekg(0, std::ios::end); // go to end of file so we can grab size with tellg.

    if (!FileStream.is_open())
    {
        throw std::runtime_error("Failed to Open File for Read/Write");
    }

    size_t FileSize = FileStream.tellg();
    std::cout << "Reading file of " << FileSize << " size\n";
    FileStream.seekg(0);

    char *pCode = new char[FileSize];

    FileStream.read(pCode, FileSize);

    return pCode;
}

GLuint LoadTexture(const char *TexPath)
{
    stbi_uc *ImageChar;
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

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(ImageChar);

    return GlTex;
}

void ErrorCallback(int Error, const char *Msg)
{
    printf("GLFW : %d %s", Error, Msg);
    throw std::runtime_error(Msg);
}

void MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar *Message, const void *UserArg)
{
    std::cout << "---------------------------Debug:\n";
    printf("Source : %s", glGetString(Source));
    printf("Type : %s", glGetString(Type));
    printf("Severity : %s", glGetString(Severity));
    printf("Message : %s", Message);
}

bool CheckKey(uint32_t Key)
{
    uint32_t KeyState = glfwGetKey(Window, Key);

    if (KeyState == GLFW_PRESS)
    {
        return true;
    }

    return false;
}

glm::vec3 Camera::GetDirVector(CamDirection Dir)
{
    if(Dir == FORWARD)
    {
        // The Camera Transform will face on the - forward axis (this is because the view matrix is equal to the inverse of this matrix, so to account for that, we multiply forward by -1)
        return  glm::normalize(Transform[2]);
    }
    else if(Dir == RIGHT)
    {
        return glm::normalize(Transform[0]);
    }
    else if(Dir == UP)
    {
        return glm::normalize(Transform[1]);
    }
}

void Camera::PollInputs(float DeltaTime)
{
    if (CheckKey(GLFW_KEY_W))
    {
        Position -= GetDirVector(FORWARD) * DeltaTime * 5.f;
    }

    if (CheckKey(GLFW_KEY_S))
    {
        Position += GetDirVector(FORWARD) * DeltaTime * 5.f;
    }

    if (CheckKey(GLFW_KEY_A))
    {
        Position -= GetDirVector(RIGHT) * DeltaTime * 5.f;
    }

    if (CheckKey(GLFW_KEY_D))
    {
        Position += GetDirVector(RIGHT) * DeltaTime * 5.f;
    }

    if (CheckKey(GLFW_KEY_SPACE))
    {
        Position += GetDirVector(UP) * DeltaTime * 5.f;
    }

    if (CheckKey(GLFW_KEY_LEFT_CONTROL))
    {
        Position -= GetDirVector(UP) * DeltaTime * 5.f;
    }

    // if(CheckKey(GLFW_KEY_I))
    // {
    //     Pitch += DeltaTime * 10.f;
    // }

    // if(CheckKey(GLFW_KEY_K))
    // {
    //     Pitch -= DeltaTime * 10.f;
    // }

    // if(CheckKey(GLFW_KEY_J))
    // {
    //     Yaw += DeltaTime * 10.f;
    // }

    // if(CheckKey(GLFW_KEY_L))
    // {
    //     Yaw -= DeltaTime * 10.f;
    // }

    double x, y;
    glfwGetCursorPos(Window, &x, &y);

    Yaw -= (x - MousePos.x) * DeltaTime * 1.f;
    Pitch -= (y - MousePos.y) * DeltaTime * 1.f;
    if(Pitch >= M_PI)
    {
        Pitch = M_PI;
    }
    else if(Pitch <= -M_PI)
    {
        Pitch = -M_PI;
    }

    MousePos = glm::vec2(x, y);
}

void PrintVec(glm::vec4& Vec)
{
    printf("%.2f %.2f, %.2f, %.2f\n", Vec[0], Vec[1], Vec[2], Vec[3]);
}

void Camera::Update()
{
    Transform = glm::translate(glm::mat4(1.0f), Position);
    Transform = glm::rotate(Transform, Yaw, glm::vec3(0.f, 1.f, 0.f));
    Transform = glm::rotate(Transform, Pitch, glm::normalize(glm::vec3(Transform[0])));
    Transform = glm::rotate(Transform, Roll, GetDirVector(FORWARD));
    Buffer.View = glm::inverse(Transform);

    // printf("%.2f %.2f %.2f\n", Roll, Pitch, Yaw);

    PrintVec(Transform[0]);
    PrintVec(Transform[1]);
    PrintVec(Transform[2]);
    PrintVec(Transform[3]);
    std::cout << "----------------------------\n";

    // PrintVec(Buffer.View[0]);
    // PrintVec(Buffer.View[1]);
    // PrintVec(Buffer.View[2]);
    // PrintVec(Buffer.View[3]);
    // std::cout << "-----------------------------------\n";

    glBindBuffer(GL_UNIFORM_BUFFER, Handle);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Buffer), &Buffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// void KeyCallBack(GLFWwindow* Window, uint32_t Key, uint32_t ScanCode, uint32_t Action, uint32_t Mods)
// {
//     if(Key == GLFW_KEY_W && Action == GLFW_)
// }

// 2D
int main()
{
    glfwInit();

    glfwSetErrorCallback(ErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    Window = glfwCreateWindow(1280, 720, "OGL Renderer", NULL, NULL);

    glfwMakeContextCurrent(Window);

    // glfwSetKeyCallback(Window, )

    std::cout << "Glew initiated with " << glewGetErrorString(glewInit()) << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    int Width, Height;
    glfwGetFramebufferSize(Window, &Width, &Height);
    glViewport(0, 0, Width, Height);

    GLint ContextFlags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &ContextFlags);
    if (!ContextFlags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        throw std::runtime_error("Not in Debug Context");
    }

    GLuint VertShader, FragShader, Program;

    VertShader = glCreateShader(GL_VERTEX_SHADER);
    const char *VertCode = LoadFile("/home/ethan/Repos/HIP/Vert.glsl");
    // std::cout << "VertShader:\n" << VertCode << std::endl;
    glShaderSource(VertShader, 1, &VertCode, NULL);
    glCompileShader(VertShader);

    int success;
    char infoLog[512];
    glGetShaderiv(VertShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(VertShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    FragShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *FragCode = LoadFile("/home/ethan/Repos/HIP/Frag.glsl");
    // std::cout << "Frag Shader:\n" << FragCode << std::endl;
    glShaderSource(FragShader, 1, &FragCode, NULL);
    glCompileShader(FragShader);

    glGetShaderiv(FragShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(FragShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    Program = glCreateProgram();
    glAttachShader(Program, VertShader);
    glAttachShader(Program, FragShader);
    glLinkProgram(Program);

    MeshLoader MainMeshLoader;
    std::vector<Mesh> Meshes = MainMeshLoader.LoadModel("/home/ethan/Repos/HIP/Snowman.dae");
    Mesh Cube = Meshes[0];

    glGenVertexArrays(1, &Cube.VertexAttribs);
    glGenBuffers(1, &Cube.VertexBuffer);
    glGenBuffers(1, &Cube.IndexBuffer);

    glBindVertexArray(Cube.VertexAttribs);

    glBindBuffer(GL_ARRAY_BUFFER, Cube.VertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Cube.Vertices.size(), NULL, GL_STATIC_DRAW);

    void *VertMem = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    memcpy(VertMem, Cube.Vertices.data(), sizeof(Vertex) * Cube.Vertices.size());

    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Cube.IndexBuffer);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * Cube.Indices.size(), NULL, GL_STATIC_DRAW);

    void *IndexMem = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
    memcpy(IndexMem, Cube.Indices.data(), sizeof(uint) * Cube.Indices.size());

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLuint CBOIndex = glGetUniformBlockIndex(Program, "CameraBuffer");
    glUniformBlockBinding(Program, CBOIndex, 0);
    if (CBOIndex & GL_INVALID_VALUE)
    {
        std::cout << "Failed to find CameraBuffer UBO\n";
    }

    Camera Camera;
    Camera.Buffer.World = glm::mat4(1.0f);
    Camera.Buffer.Projection = glm::mat4(1.0f);

    Camera.Buffer.Projection = glm::perspective(45.f, (float)Width / (float)Height, 0.1f, 100.f);

    Camera.Position = glm::vec3(0.f, 0.f, 10.f);

    glGenBuffers(1, &Camera.Handle);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    double DeltaTime;
    int CurrentTime;
    int PreviousTime;

    CurrentTime = clock();

    while (!glfwWindowShouldClose(Window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        PreviousTime = CurrentTime;
        CurrentTime = clock();

        DeltaTime = 1.f / (CurrentTime - PreviousTime);

        glfwPollEvents();
        Camera.PollInputs(DeltaTime);

        Camera.Update();

        glUseProgram(Program);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, Camera.Handle);
        glBindVertexArray(Cube.VertexAttribs);
        glDrawElements(GL_TRIANGLES, Cube.Indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(Window);
    }

    glDeleteVertexArrays(1, &Cube.VertexAttribs);
    glDeleteBuffers(1, &Cube.VertexBuffer);
    glDeleteBuffers(1, &Cube.IndexBuffer);
    glDeleteProgram(Program);

    // while(!glfwWindowShouldClose(Window))
    // {
    //     glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    //     glClear(GL_COLOR_BUFFER_BIT);

    //     glUseProgram(Program);
    //     glBindVertexArray(VertAttribs);
    //     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //     // glBegin(GL_TRIANGLES);
    //     // glVertex2f(-0.5f, -0.5f);
    //     // glVertex2f(0, 0.5f);
    //     // glVertex2f(0.5f, -0.5f);
    //     // glEnd();

    //     glfwSwapBuffers(Window);
    //     glfwPollEvents();
    // }
}
