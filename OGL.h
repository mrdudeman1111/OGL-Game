#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>

typedef std::vector<Mesh> MeshList;

std::string LoadFile(const char* ShaderPath);

GLuint LoadTexture(const char* TexPath);

struct Vertex
{
    public:
    // Cartesian Coordinates
    std::vector<float> vLoc;
    std::vector<float> vCol;
    std::vector<float> vNorm;
};

struct Mesh
{
    public:
    std::vector<Vertex> Vertices;
    glm::mat4 Transform;
    std::string Name;
};

class MeshLoader
{
    public:
    void LoadNode(aiNode* Node)
    MeshList LoadModel(const char* ModelPath);

    private:
    Assimp::Importer AssImporter;
};
