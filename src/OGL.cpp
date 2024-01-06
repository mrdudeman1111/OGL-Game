#include <iostream>
#include <fstream>
#include <locale>
#include <stdexcept>
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

GLFWwindow *Window;

void ErrorCallback(int Error, const char *Msg)
{
    printf("GLFW : %d %s", Error, Msg);
    throw std::runtime_error(Msg);
}

void GLAPIENTRY OpenglCallback(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, const GLchar* Message, const void* UserParam)
{
  printf("OPENGL Error : %s\n", Message);
}

const char* LoadFile(const char *ShaderPath, GLint& Size)
{
    std::ifstream ShaderFile(ShaderPath, std::ios::ate);

    if(!ShaderFile.is_open())
    {
      throw std::runtime_error("Failed to open shader file for reading");
    }

    size_t ShaderSize = ShaderFile.tellg();
    Size = ShaderSize;
    std::cout << "Reading file of size: " << ShaderSize << '\n';
    ShaderFile.seekg(std::ios::beg);

    char* pCode = new char[ShaderSize];

    ShaderFile.read(pCode, ShaderSize);
    ShaderFile.close();

    return pCode;
}

void CreateProgram(GLuint& Program, const char* VertPath, const char* FragPath)
{
    GLuint VertShader, FragShader;
    GLuint Err;

    int Success;
    char InfoLog[512];

    GLint VertSize = 0;
    const char* VertCode = LoadFile(VertPath, VertSize);
    VertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertShader, 1, &VertCode, &VertSize);
    glCompileShader(VertShader);

    glGetShaderiv(VertShader, GL_COMPILE_STATUS, &Success);
    if(Success != GL_TRUE)
    {
        glGetShaderInfoLog(VertShader, 512, NULL, InfoLog);
        std::cout << "Error from Vert Shader:\n" << InfoLog << '\n';
    }

    delete VertCode;

    GLint FragSize = 0;
    const char* FragCode = LoadFile(FragPath, FragSize);
    FragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragShader, 1, &FragCode, &FragSize);
    glCompileShader(FragShader);

    glGetShaderiv(FragShader, GL_COMPILE_STATUS, &Success);
    if(Success != GL_TRUE)
    {
        glGetShaderInfoLog(FragShader, 512, NULL, InfoLog);
        std::cout << "Error from Frag shader:\n" << InfoLog << '\n';
    }

    delete FragCode;

    Program = glCreateProgram();
    glAttachShader(Program, VertShader);
    glAttachShader(Program, FragShader);
    glLinkProgram(Program);
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
    if(Pitch >= M_PI/2.f)
    {
        Pitch = M_PI/2.f;
    }
    else if(Pitch <= -M_PI/2.f)
    {
        Pitch = -M_PI/2.f;
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
    Transform = glm::rotate(Transform, Pitch, glm::vec3(1.f, 0.f, 0.f));
    Transform = glm::rotate(Transform, Roll, glm::vec3(0.f, 0.f, 1.f));
    Buffer.View = glm::inverse(Transform);

    glBindBuffer(GL_UNIFORM_BUFFER, Handle);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Buffer), &Buffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void VertexAttribs()
{
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);
}

void FrameBuffer::Create(uint32_t Width, uint32_t Height)
{
    glGenFramebuffers(1, &FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

    glGenTextures(1, &PosAtt);
    glBindTexture(GL_TEXTURE_2D, PosAtt);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PosAtt, 0);

    glGenTextures(1, &NormAtt);
    glBindTexture(GL_TEXTURE_2D, NormAtt);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NormAtt, 0);

    glGenTextures(1, &ColorAtt);
    glBindTexture(GL_TEXTURE_2D, ColorAtt);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, ColorAtt, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint Attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, Attachments);
}

// 3D
int main()
{
    glfwInit();

    glfwSetErrorCallback(ErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    Window = glfwCreateWindow(1280, 720, "OGL Renderer", NULL, NULL);

    glfwMakeContextCurrent(Window);

    std::cout << "Glew initiated with " << glewGetErrorString(glewInit()) << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(OpenglCallback, 0);

    int Width, Height;
    glfwGetFramebufferSize(Window, &Width, &Height);
    glViewport(0, 0, Width, Height);

    Program PrimaryProgram;

    std::cout << "Creating a program with " << SourceDir"/Shaders/Vert.glsl" << " and " << SourceDir"/Shaders/Frag.glsl\n";
    CreateProgram(PrimaryProgram.Prog, SourceDir "/Shaders/Vert.glsl", SourceDir "/Shaders/Frag.glsl");

    MeshLoader MainMeshLoader;
    std::vector<Mesh> Meshes = MainMeshLoader.LoadModel(SourceDir "/Models/ColorCube.dae");
    Mesh Cube = Meshes[0];

    Cube.SetupVertAttribs(&VertexAttribs);

    GLint CBOIndex = glGetUniformBlockIndex(PrimaryProgram.Prog, "CameraBuffer");
    glUniformBlockBinding(PrimaryProgram.Prog, CBOIndex, 0);
    if(glGetError() != GL_NO_ERROR)
    {
        std::cout << "Failed to find CameraBuffer UBO: " << CBOIndex << '\n';
    }

    Camera Camera;
    Camera.Buffer.World = glm::mat4(1.0f);
    Camera.Buffer.Projection = glm::mat4(1.0f);

    Camera.Buffer.Projection = glm::perspective(45.f, (float)Width / (float)Height, 0.1f, 100.f);

    Camera.Position = glm::vec3(0.f, 0.f, 10.f);

    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // Set starting mousePos
        glfwPollEvents();
        Camera.PollInputs(0.16);

        Camera.Yaw = 0.f;
        Camera.Roll = 0.f;
        Camera.Pitch = 0.f;

    glGenBuffers(1, &Camera.Handle);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    double DeltaTime;
    int CurrentTime;
    int PreviousTime;

    CurrentTime = clock();

    glUseProgram(PrimaryProgram.Prog);
    glUniform1i(glGetUniformLocation(PrimaryProgram.Prog, "MeshTexture"), 0);

    while (!glfwWindowShouldClose(Window))
    {
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        PreviousTime = CurrentTime;
        CurrentTime = clock();

        DeltaTime = 1.f / (CurrentTime - PreviousTime);

        glfwPollEvents();
        Camera.PollInputs(DeltaTime);

        Camera.Update();

        glUseProgram(PrimaryProgram.Prog);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, Camera.Handle);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Cube.Texture);

        glBindVertexArray(Cube.VertexAttribs);
        glDrawElements(GL_TRIANGLES, Cube.Indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(Window);
    }

    glDeleteVertexArrays(1, &Cube.VertexAttribs);
    glDeleteBuffers(1, &Cube.VertexBuffer);
    glDeleteBuffers(1, &Cube.IndexBuffer);
    glDeleteProgram(PrimaryProgram.Prog);

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
