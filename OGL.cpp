#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Vertex.h>

#include <OGL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

std::string LoadFile(const char *ShaderPath)
{
    std::string pCode;

    std::ifstream FileStream(ShaderPath, std::ios::in);

    if (!FileStream.is_open())
    {
        throw std::runtime_error("Failed to Open File for Read/Write");
    }

    size_t FileSize = FileStream.tellg();
    FileStream.seekg(0);

    FileStream.read(pCode.data(), FileSize);

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

    return GlTex;

    stbi_image_free(ImageChar);
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
            for(uint32_t i = 0; i < aiMesh->mFaces->mNumIndices; i++)
            {
                lMesh.Indices.push_back(aiMesh->mFaces->mIndices[i]);
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

int main()
{
    if(!glfwInit())
    {
        throw std::runtime_error("Failed to Initiate glfw");
    }

    glfwWindowHint(GLFW_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_VERSION_MINOR, 0);
    GLFWwindow *Window = glfwCreateWindow(1280, 720, "Opengl Sample", NULL, NULL);

    glfwMakeContextCurrent(Window);

    try
    {
        glewInit();
    }
    catch(GLenum Error)
    {
        std::cout << glewGetErrorString(Error);
        throw std::runtime_error("Failed to Initiate glew");
    }

    glViewport(0, 0, 1280, 720);

    GLuint VertShader = glCreateShader(GL_VERTEX_SHADER);

    std::string VertCode = LoadFile("/home/ethan/Repos/HIP/Vert.glsl");
    const char *pVertCode = VertCode.c_str();

    glShaderSource(VertShader, 1, &pVertCode, NULL);
    glCompileShader(VertShader);

    GLuint FragShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string FragCode = LoadFile("/home/ethan/Repos/HIP/Frag.glsl");
    const char *pFragCode = FragCode.c_str();

    glShaderSource(FragShader, 1, &pFragCode, NULL);
    glCompileShader(FragShader);

    GLuint Program = glCreateProgram();
    glAttachShader(Program, VertShader);
    glAttachShader(Program, FragShader);
    glLinkProgram(Program);

    glClearColor(0, 0, 0, 1);

    glUseProgram(Program);

    // Load Meshes

        MeshLoader Loader;
        std::vector<Mesh> Meshes(0);
        Meshes = Loader.LoadModel("/home/ethan/Repos/HIP/Cube.fbx");
        Meshes.shrink_to_fit();
        std::cout << "Loaded " << Meshes.size() << " Meshes\n";
        Mesh Cube;
        Cube = Meshes[0];

        glGenVertexArrays(1, &Cube.VertexAttribs);
        glGenBuffers(1, &Cube.VertexBuffer);
        glGenBuffers(1, &Cube.IndexBuffer);

        glBindVertexArray(Cube.VertexAttribs);

            glBindBuffer(GL_ARRAY_BUFFER, Cube.VertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(Cube.Vertices), Cube.Vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Cube.IndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Cube.Indices), Cube.Indices.data(), GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
                // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
                // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                // glEnableVertexAttribArray(1);
                // glEnableVertexAttribArray(2);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);


    // Setup Camera
        CameraBuffer Camera;
        Camera.World = glm::mat4(1.0f);
        Camera.Projection = glm::mat4(1.0f);
        Camera.Projection = glm::perspective(glm::radians(45.f), (float)1280/720, 0.1f, 1000.f);
        Camera.View = glm::translate(glm::mat4(1.f), glm::vec3(-10.f, 0.f, 0.f));
        Camera.View = glm::lookAt(glm::vec3(-10.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.0f));

        uint CBOIndex = glGetUniformBlockIndex(Program, "CBO");
        glUniformBlockBinding(Program, CBOIndex, 0);

        GLuint CBO;
        glGenBuffers(1, &CBO);
        glBindBuffer(GL_UNIFORM_BUFFER, CBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraBuffer), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferRange(GL_UNIFORM_BUFFER, 0, CBO, 0, sizeof(CameraBuffer));

        glBindBuffer(GL_UNIFORM_BUFFER, CBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraBuffer), &Camera);

    glClearColor(0, 0, 0, 1);

    while (!glfwWindowShouldClose(Window))
    {
        glClearColor(0.25f, 0.25f, 0.25f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(Program);
        glBindVertexArray(Cube.VertexAttribs);
        glDrawElements(GL_TRIANGLES, Cube.Vertices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &Cube.VertexAttribs);
    glDeleteBuffers(1, &Cube.VertexBuffer);
    glDeleteBuffers(1, &Cube.IndexBuffer);
    glDeleteProgram(Program);

    glfwDestroyWindow(Window);

    glfwTerminate();

    return 0;
}
