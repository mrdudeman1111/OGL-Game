#include <Mesh.h>

#include <assimp/postprocess.h>
#include <assimp/Vertex.h>

void Mesh::SetupVertAttribs()
{
    glGenVertexArrays(1, &VertexAttribs);
    glGenBuffers(1, &VertexBuffer);
    glGenBuffers(1, &IndexBuffer);

    glBindVertexArray(VertexAttribs);

    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), NULL, GL_STATIC_DRAW);

    void* VertMem = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    memcpy(VertMem, Vertices.data(), sizeof(Vertex) * Vertices.size());

    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * Indices.size(), NULL, GL_STATIC_DRAW);

    void* IndexMem = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
    memcpy(IndexMem, Indices.data(), sizeof(uint) * Indices.size());

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh MeshLoader::ParseMesh(aiMesh* aiMesh)
{
    Mesh lMesh;
    lMesh.Name = aiMesh->mName.C_Str();
    std::cout << "Parsing Mesh with name: " << aiMesh->mName.C_Str() << std::endl;

    for(uint32_t i = 0; i < aiMesh->mNumVertices; i++)
    {
        Vertex Vert;
        Vert.vPos.x = aiMesh->mVertices[i].x;
        Vert.vPos.y = aiMesh->mVertices[i].y;
        Vert.vPos.z = aiMesh->mVertices[i].z;

        // if(aiMesh->HasVertexColors(i))
        // {
        //     Vert.vCol.r = aiMesh->mColors[i]->r;
        //     Vert.vCol.g = aiMesh->mColors[i]->g;
        //     Vert.vCol.b = aiMesh->mColors[i]->b;
        //     Vert.vCol.a = aiMesh->mColors[i]->a;
        // }

        // if(aiMesh->HasNormals())
        // {
        //     Vert.vNorm.x = aiMesh->mNormals[i].x;
        //     Vert.vNorm.y = aiMesh->mNormals[i].y;
        //     Vert.vNorm.z = aiMesh->mNormals[i].z;
        // }

        // if(aiMesh->HasTextureCoords(i))
        // {
        //     Vert.vCoord.x = aiMesh->mTextureCoords[i]->x;
        //     Vert.vCoord.y = aiMesh->mTextureCoords[i]->y;
        //     Vert.vCoord.z = aiMesh->mTextureCoords[i]->z;
        // }

        lMesh.Vertices.push_back(Vert);
    }

    if(aiMesh->HasFaces())
    {
        for(uint32_t i = 0; i < aiMesh->mNumFaces; i++)
        {
            for(uint32_t x = 0; x < aiMesh->mFaces[i].mNumIndices; x++)
            {
                lMesh.Indices.push_back(aiMesh->mFaces[i].mIndices[x]);
            }
        }
    }

    std::cout << "Imported Mesh Named " << lMesh.Name << std::endl;

    return lMesh;
}

void MeshLoader::LoadNode(aiNode* Node, aiMesh* aiMeshes, std::vector<Mesh>* Meshes)
{
    if(Node->mNumChildren != 0)
    {
        for (uint32_t i = 0; i < Node->mNumChildren; i++)
        {
            std::cout << "Loading children of Node Named: " << Node->mChildren[i]->mName.C_Str() << std::endl;
            LoadNode(Node->mChildren[i], aiMeshes, Meshes);
        }
    }

    if(Node->mNumMeshes != 0)
    {
        for(uint32_t i = 0; i < Node->mNumMeshes; i++)
        {
            std::cout << "Loading Mesh of Node Named: " << aiMeshes[Node->mMeshes[i]].mName.C_Str() << std::endl;
            Mesh lMesh = ParseMesh(&aiMeshes[Node->mMeshes[i]]);
            Meshes->push_back(lMesh);
        }
    }

    return;
}

std::vector<Mesh> MeshLoader::LoadModel(const char *ModelPath)
{
    std::vector<Mesh> Meshes;
    const aiScene* Scene = AssImporter.ReadFile(ModelPath, aiProcess_Triangulate);
    if(Scene->mNumMeshes != 0)
    {
        LoadNode(Scene->mRootNode, *Scene->mMeshes, &Meshes);
    }

    return Meshes;
}
