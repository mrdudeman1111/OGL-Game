#include <iostream>
#include <vector>

#include <Mesh.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

const char* LoadFile(const char* ShaderPath);

GLuint LoadTexture(const char* TexPath);

struct CameraBuffer
{
    public:
    glm::mat4 World;
    glm::mat4 View;
    glm::mat4 Projection;
};

enum CamDirection
{
    FORWARD,
    UP,
    RIGHT
};

class Camera
{
    public:
    CameraBuffer Buffer;
    GLuint Handle;
    glm::vec3 Position;
    glm::vec4 Rotation;

    void Update();
    glm::vec3 GetDirVector(CamDirection Dir);
    void PollInputs(float DeltaTime);
};
