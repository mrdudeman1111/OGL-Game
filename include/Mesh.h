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
    glm::vec3 vNorm;
    glm::vec2 vCoord;
};

GLuint LoadTexture(const char *TexPath);

struct Mesh
{
    public:
    std::vector<Vertex> Vertices;
    std::vector<uint> Indices;

    GLuint VertexBuffer;
    GLuint IndexBuffer;
    GLuint VertexAttribs;

    GLuint Texture;

    void SetupVertAttribs(void (*VertPointers)());
    void Draw(GLuint Program);

    std::string Name;
    glm::mat4 Transform;
};

class MeshLoader
{
    public:
    std::vector<Mesh> LoadModel(const char* ModelPath);

    private:
    Assimp::Importer AssImporter;

    void LoadNode(aiNode* Node, aiMesh* aiMeshes, aiMaterial** Materials, std::vector<Mesh>* Meshes);
    Mesh ParseMesh(aiMesh* aiMesh, aiMaterial* Material);
};
