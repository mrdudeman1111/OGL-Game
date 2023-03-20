#include <vector>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

struct Vertex
{
    // Cartesian Coordinates
    glm::vec3 vPos;
    // glm::vec4 vCol;
    // glm::vec3 vNorm;
    // glm::vec3 vCoord;
};


struct Mesh
{
    public:
    std::vector<Vertex> Vertices;
    std::vector<uint> Indices;

    GLuint VertexBuffer;
    GLuint IndexBuffer;
    GLuint VertexAttribs;

    void SetupVertAttribs();

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