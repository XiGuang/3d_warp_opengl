//
// Created by Admin on 2023/11/28.
//

#ifndef INC_3D_WARP_OPENGL_MODEL_MANAGER_H
#define INC_3D_WARP_OPENGL_MODEL_MANAGER_H

#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <mutex>
#include <glm/glm.hpp>
#include "../model/model.h"
#include "../shader/shader.h"

class ModelManager {
public:
    ModelManager() = default;

    ModelManager(const std::string &path, const std::string &name);

    ~ModelManager() = default;

    bool loadModel(const std::string &name, const std::string &path, glm::mat4 model_matrix = glm::mat4(1.0f));

    bool loadModels(const std::vector<std::filesystem::path> &model_paths, const std::vector<std::string> &model_names,int threads_num);

    bool drawModel(const std::string &name,Shader &shader);

    void drawAllModel(Shader &shader);

    Model &getModel(const std::string &name);

    unsigned int getModelNum(){return _models.size();}

    bool setModelMatrix(const std::string &name, const glm::mat4 &model_matrix);

    bool getModelMatrix(const std::string &name, glm::mat4 &model_matrix);

    void clear();

private:
    // model_name -> (model_matrix,model)
    std::unordered_map<std::string, std::pair<glm::mat4,std::shared_ptr<Model>>> _models;

    static std::mutex _mutex;
};


#endif //INC_3D_WARP_OPENGL_MODEL_MANAGER_H
