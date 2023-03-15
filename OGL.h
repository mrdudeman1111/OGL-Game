#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>

#include <assimp/scene.h>

#include <glm/glm.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>


std::string LoadFile(const char* ShaderPath);

GLuint LoadTexture(const char* TexPath);

struct Vertex
{
    public:
    // Cartesian Coordinates
     glm::vec3 vPos;
    // glm::vec4 vCol;
    // glm::vec3 vNorm;
    // glm::vec3 vCoord;
};

struct CameraBuffer
{
    public:
    glm::mat4 World, View, Projection;
};

struct Mesh
{
    public:
    std::vector<Vertex> Vertices;
    std::vector<uint> Indices;

    GLuint VertexBuffer;
    GLuint IndexBuffer;
    GLuint VertexAttribs;

    std::string Name;
    glm::mat4 Transform;
};

class MeshLoader
{
    public:
    std::vector<Mesh> LoadModel(const char* ModelPath);

    private:
    Assimp::Importer AssImporter;

    void LoadNode(aiNode* Node, aiMesh* aiMeshes, std::vector<Mesh>* Meshes);
    Mesh ParseMesh(aiMesh* aiMesh);
};
