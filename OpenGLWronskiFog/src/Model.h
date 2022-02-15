#ifndef MODEL_H
#define MODEL_H

/* 
    Built using LearnOpenGL's model loading and rendering tutorial (written by
    Joey de Vries), available at: https://learnopengl.com/Model-Loading/Model 
*/

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image/stb_image.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <iostream>
#include <vector>

class Model
{
public:
    std::vector<Texture>    m_textures;
    std::vector<Mesh>       m_meshes;
    std::string             m_directory;

    Model() {};
    Model(const std::string& filePath) {
        setModel(filePath);
    }

    // Position setters (overwrites previous transforms):
    inline void setPosition(float x, float y, float z) {
        m_worldMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    }
    inline void setPosition(glm::vec3 newPos) {
        m_worldMat = glm::translate(glm::mat4(1.0f), newPos);
    }

    // Translation functions:
    inline void translate(float x, float y, float z) {
        m_worldMat = glm::translate(m_worldMat, glm::vec3(x, y, z));
    }
    inline void translate(glm::vec3 newPos) {
        m_worldMat = glm::translate(m_worldMat, newPos);
    }

    // Scale functions:
    inline void scale(float uniformScale) {
        m_worldMat = glm::scale(m_worldMat, glm::vec3(uniformScale));
    }
    inline void scale(float x, float y, float z) {
        m_worldMat = glm::scale(m_worldMat, glm::vec3(x, y, z));
    }

    // Rotation functions (degrees):
    inline void rotate(float x, float y, float z, float angleDegrees) {
        m_worldMat = glm::rotate(m_worldMat, glm::radians(angleDegrees), glm::vec3(x, y, z));
    }
    inline void rotate(glm::vec3 rot, float angleDegrees) {
        m_worldMat = glm::rotate(m_worldMat, glm::radians(angleDegrees), rot);
    }

    glm::mat4 getWorldMat() const { return m_worldMat; }

    inline void setModel(const std::string& filePath)
    {
        // If this object already contains a model, empty mesh and texture vectors:
        if (m_meshes.size() || m_textures.size())
        {
            m_meshes.clear();
            m_textures.clear();
        }
        std::cout << "Loading model " << filePath << "..." << std::endl;
        loadModel(filePath);
    }

    // Call OpenGL draw functions for each mesh in this model:
    inline void draw(Shader& shader)
    {
        for (size_t i = 0; i < m_meshes.size(); i++)
            m_meshes[i].draw(shader);
    }

    // Load a model using Assimp and store model data in vector of Mesh objects:
    void loadModel(std::string const& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
            return;
        }
        m_directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);

        std::cout << "Successfully loaded " << path << "!" << std::endl;
    }

private:
    glm::mat4 m_worldMat = glm::mat4(1.0f);

    // Process Assimp scene nodes recursively, starting from the root:
    void processNode(aiNode* node, const aiScene* scene)
    {
        // Process meshes at the current node:
        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(processMesh(mesh, scene));
        }
        // After all meshes in this node have been processed (if any), process meshes in child nodes:
        for (size_t i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene);
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<VAO::Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            VAO::Vertex vertex;
            glm::vec3 vector;

            // Position:
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            // Normal (if present):
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }

            // Tex coords (if present):
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;
            }
            else
                vertex.texCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // Retrieve vertex indices by processing each mesh face (triangle):
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Process materials and store texture map data:
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // Diffuse/albedo:
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // Specular:
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        return Mesh(vertices, indices, textures);
    }

    // Check and load all material textures of a given type:
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        for (size_t i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            bool skip = false;
            for (unsigned int j = 0; j < m_textures.size(); j++)
            {
                // If texture hasn't been loaded yet, push to vector and mark it as loaded:
                if (std::strcmp(m_textures[j].path.c_str(), str.C_Str()) == 0)
                {
                    textures.push_back(m_textures[j]);
                    skip = true;
                    break;
                }
            }
            // Populate texture object with data, if the texture hasn't been loaded already:
            if (!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), m_directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                m_textures.push_back(texture);
            }
        }
        return textures;
    }

    // Read texture data with stb_image:
    unsigned int TextureFromFile(const char* path, const std::string& directory)
    {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        GLuint textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        // If no data was found, output error message:
        else
            std::cout << "IMAGE LOADING ERROR: Texture failed to load at path " << path << std::endl;

        stbi_image_free(data);
        return textureID;
    }
};

#endif