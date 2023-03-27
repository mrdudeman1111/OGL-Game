#include <iostream>
#include <vector>

#include <Mesh.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

const char* LoadFile(const char* ShaderPath);

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
    glm::mat4 Transform; // This matrix is the "Camera" Matrix and to get the view matrix, We take the inverse of this transform. After we have that we stuff it in the buffer and send it to the GPU
    glm::vec3 Position = glm::vec3(0.f, 0.f, -10.f);
    float Yaw = 0;
    float Roll = 0;
    float Pitch = 0;
    glm::vec2 MousePos;

    void Update();
    glm::vec3 GetDirVector(CamDirection Dir);
    void PollInputs(float DeltaTime);
};
