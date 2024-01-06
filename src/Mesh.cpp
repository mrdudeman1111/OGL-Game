#include <Mesh.h>

#include <assimp/postprocess.h>
#include <assimp/Vertex.h>

#include <functional>

#include <sail-c++/sail-c++.h>
#include <stdexcept>

#define ModelsPath SourceDir"/Models/"

GLuint LoadTexture(const char *TexPath)
{
    // int Width, Height, Channels;
    // u_char* ImageData = stbi_load(TexPath, &Width, &Height, &Channels, 0);

    sail::image Image(TexPath);

    if(!Image.is_valid())
    {
      throw std::runtime_error("Failed to load Texture");
    }

    GLuint GlTex;
    glGenTextures(1, &GlTex);
    glBindTexture(GL_TEXTURE_2D, GlTex);

    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Image.width(), Image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Image.pixels());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // stbi_image_free(ImageData);

    printf("Loaded Texture with %d Width, %d Height.\n", Image.width(), Image.height());

    Image.~image();

    return GlTex;
}

void Mesh::SetupVertAttribs(void (*VertPointers)())
{
    glGenVertexArrays(1, &VertexAttribs);
    glGenBuffers(1, &VertexBuffer);
    glGenBuffers(1, &IndexBuffer);

    glBindVertexArray(VertexAttribs);

    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);
        VertPointers();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * Indices.size(), Indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Draw(GLuint Program)
{
    glUniform1i(glGetUniformLocation(Program, "MeshTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);

    glBindVertexArray(VertexAttribs);

    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);

    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, nullptr);
}

Mesh MeshLoader::ParseMesh(aiMesh* aiMesh, aiMaterial* Material)
{
    Mesh lMesh;
    lMesh.Name = aiMesh->mName.C_Str();

    for(uint32_t i = 0; i < aiMesh->mNumVertices; i++)
    {
        Vertex Vert;
        Vert.vPos.x = aiMesh->mVertices[i].x;
        Vert.vPos.y = aiMesh->mVertices[i].z;
        Vert.vPos.z = aiMesh->mVertices[i].y;

        // if(aiMesh->mColors[i])
        // {
        //     Vert.vCol.r = aiMesh->mColors[i]->r;
        //     Vert.vCol.g = aiMesh->mColors[i]->g;
        //     Vert.vCol.b = aiMesh->mColors[i]->b;
        //     Vert.vCol.a = aiMesh->mColors[i]->a;
        // }

        if(aiMesh->HasNormals())
        {
            Vert.vNorm.x = aiMesh->mNormals[i].x;
            Vert.vNorm.y = aiMesh->mNormals[i].y;
            Vert.vNorm.z = aiMesh->mNormals[i].z;
        }

        // aiMesh->mNumCoordinateChannels()
        if(aiMesh->mNumUVComponents != 0)
        {
            Vert.vCoord.x = aiMesh->mTextureCoords[0][i].x;
            Vert.vCoord.y = aiMesh->mTextureCoords[0][i].y;
        }

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

    std::cout << "Mesh has " << Material->GetTextureCount(aiTextureType_DIFFUSE) << " Diffuse textures and " << Material->GetTextureCount(aiTextureType_BASE_COLOR) << " Base Color Textures" << std::endl;

    if(Material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
    {
        aiString aiTexPath;
        Material->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath);
        std::string TexPath = ModelsPath;
        TexPath.append(aiTexPath.C_Str());
        std::cout << "Importing texture from " << TexPath << '\n';
        lMesh.Texture = LoadTexture(TexPath.c_str());
    }

    std::cout << "Imported Mesh Named " << lMesh.Name << std::endl;

    return lMesh;
}

void MeshLoader::LoadNode(aiNode* Node, aiMesh* aiMeshes, aiMaterial** Materials, std::vector<Mesh>* Meshes)
{
    if(Node->mNumChildren != 0)
    {
        for (uint32_t i = 0; i < Node->mNumChildren; i++)
        {
            LoadNode(Node->mChildren[i], aiMeshes, Materials, Meshes);
        }
    }

    if(Node->mNumMeshes != 0)
    {
        for(uint32_t i = 0; i < Node->mNumMeshes; i++)
        {

            // Get Material Index from the current Mesh.
            uint32_t MatIndex = aiMeshes[Node->mMeshes[i]].mMaterialIndex;

            // Parse Mesh, Passing a reference to the current 
            Mesh lMesh = ParseMesh(&aiMeshes[Node->mMeshes[i]], Materials[MatIndex]);
            Meshes->push_back(lMesh);
        }
    }

    return;
}

std::vector<Mesh> MeshLoader::LoadModel(const char *ModelPath)
{
    std::vector<Mesh> Meshes;
    const aiScene* Scene = AssImporter.ReadFile(ModelPath, aiProcess_Triangulate);
    if(Scene->mNumMeshes)
    {
        LoadNode(Scene->mRootNode, *Scene->mMeshes, Scene->mMaterials, &Meshes);
    }

    return Meshes;
}


