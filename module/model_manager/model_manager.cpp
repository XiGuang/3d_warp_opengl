//
// Created by Admin on 2023/11/28.
//

#include "model_manager.h"
#include <glog/logging.h>
#include <thread>

std::mutex ModelManager::_mutex;

bool ModelManager::loadModel(const std::string &name, const std::string &path, glm::mat4 model_matrix) {
    if(_models.find(name)!=_models.end()){
        LOG(ERROR)<<"model "<<name<<" already exists";
        return false;
    }
    _models.emplace(name, std::make_pair(model_matrix,std::make_shared<Model>(path)));
    return true;
}

bool ModelManager::drawModel(const std::string &name,Shader &shader) {
    if(_models.find(name)==_models.end()){
        LOG(ERROR)<<"model "<<name<<" not exists";
        return false;
    }
    shader.setMat4("model", _models.at(name).first);
    _models.at(name).second->Draw(shader);
    return true;
}

void ModelManager::drawAllModel(Shader &shader) {
    for(auto &model:_models){
        shader.setMat4("model", model.second.first);
        model.second.second->Draw(shader);
    }
}

Model &ModelManager::getModel(const std::string &name) {
    return *_models.at(name).second;
}

bool ModelManager::setModelMatrix(const std::string &name, const glm::mat4 &model_matrix) {
    if(_models.find(name)==_models.end()){
        LOG(ERROR)<<"model "<<name<<" not exists";
        return false;
    }
    _models.at(name).first=model_matrix;
    return true;
}

void ModelManager::clear() {
    _models.clear();
}

bool ModelManager::getModelMatrix(const std::string &name, glm::mat4 &model_matrix) {
    if(_models.find(name)==_models.end()){
        LOG(ERROR)<<"model "<<name<<" not exists";
        return false;
    }
    model_matrix=_models.at(name).first;
    return true;
}

bool ModelManager::loadModels(const std::vector<std::filesystem::path> &model_paths,
                              const std::vector<std::string> &model_names, int threads_num) {
    if(model_paths.size()!=model_names.size()){
        LOG(ERROR)<<"model_paths.size()!=model_names.size()";
        return false;
    }

//    std::vector<std::thread> threads;
//    for (int i = 0; i < threads_num; ++i) {
//        threads.emplace_back([&,i](){
//            for (int j = i; j < model_paths.size(); j += threads_num) {
//                if(_models.find(model_names[j])!=_models.end()){
//                    LOG(ERROR)<<"model "<<model_names[j]<<" already exists";
//                    continue;
//                }
////                auto temp_model=std::make_shared<Model>(model_paths[j].string());
//                // 上线程锁
//                std::lock_guard<std::mutex> lockGuard(_mutex);
//                _models.emplace(model_names[j], std::make_pair(glm::mat4(1.0f),std::make_shared<Model>(model_paths[j].string())));
//                LOG(INFO)<<"loaded model "<<model_names[j];
//            }
//        });
//    }
//    for (auto &thread : threads) {
//        thread.join();
//    }
    // 单线程
    for (int j = 0; j < model_paths.size(); ++j) {
        if(_models.find(model_names[j])!=_models.end()){
            LOG(ERROR)<<"model "<<model_names[j]<<" already exists";
            continue;
        }
        _models.emplace(model_names[j], std::make_pair(glm::mat4(1.0f), std::make_shared<Model>(model_paths[j].string())));
        LOG(INFO)<<"loaded model "<<model_names[j];
    }
    return true;
}

ModelManager::ModelManager(const std::string &path, const std::string &name) {
    Model::SetDefaultTexture(name,path);
}
