//
// Created by Admin on 2023/11/22.
//

#include <glad/glad.h>
#include "model.h"

#include <sstream>
#include <glog/logging.h>
#include "../stb_image/stb_image.h"

Texture Model::defaultTexture{};

unsigned int TextureFromFile(const char *name, const std::string &directory, bool gamma) {
    std::string filename = std::string(name);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format;
        if (nrComponents == 1) {
            format = GL_RED;
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
        }
        else if (nrComponents == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 纹理环绕方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// x轴
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// y轴
        // 纹理过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// 缩小
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// 放大

        stbi_image_free(data);
    }
    else {
        LOG(ERROR) << "Texture failed to load at path: " << filename;
        stbi_image_free(data);
    }

    return textureID;
}

//Texture Model::defaultTexture{TextureFromFile("1.jpg", "../model",false),"texture_diffuse","1.jpg"};

Model::Model(std::string const &path, bool gamma) : gammaCorrection(gamma) {
    loadModel(path);
}

void Model::Draw(Shader &shader) {
    for (auto & mesh : meshes) {
        mesh.Draw(shader);
    }
}

void Model::loadModel(std::string const &path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate |
            aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode) {
        LOG(ERROR) << "ERROR::ASSIMP::" << importer.GetErrorString();
        return;
    }

    // retrieve the directory path of the filepath
    if(path.find('/') != std::string::npos)
        directory = path.substr(0, path.find_last_of('/'));
    else if(path.find('\\') != std::string::npos)
        directory = path.substr(0, path.find_last_of('\\'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // 处理节点所有的网格（如果有的话）
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // 处理顶点
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};
        // 处理顶点位置、法线和纹理坐标
        glm::vec3 vector;
        // 顶点位置
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // 顶点法线
        if(mesh->HasNormals()){
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }else{
            vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        // 纹理坐标
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            // 一个顶点最多可以有8个不同的纹理坐标，我们只关心第一个纹理坐标
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        // 切线
        if(mesh->HasTangentsAndBitangents()) {
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // 双切线
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }else{
            vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            vertex.Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    // 处理索引
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return {vertices, indices, textures};
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;

        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    // Check if textures vector is empty, add a placeholder texture
    if (textures.empty()) {
        Texture placeholder(defaultTexture);
        placeholder.type = typeName;
        // You might want to assign a default texture ID or other properties to the placeholder
        textures.push_back(placeholder);
    }
    return textures;
}



