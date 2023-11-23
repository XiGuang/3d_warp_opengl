//
// Created by Admin on 2023/11/22.
//

#ifndef INC_3D_WARP_OPENGL_MODEL_H
#define INC_3D_WARP_OPENGL_MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include "../mesh/mesh.h"
#include "../shader/shader.h"

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model {
public:
    // model data
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;
    static Texture defaultTexture;
    bool gammaCorrection;

    static void SetDefaultTexture(const char *path, const std::string &directory, bool gamma = false){
        defaultTexture.id = TextureFromFile(path, directory, gamma);
        defaultTexture.type = "texture_diffuse";
        defaultTexture.path = path;
    }

    // constructor, expects a filepath to a 3D model.
    explicit Model(std::string const &path, bool gamma = false);

    // draws the model, and thus all its meshes
    void Draw(Shader &shader, GLuint depthMap);

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName);

};



#endif //INC_3D_WARP_OPENGL_MODEL_H
