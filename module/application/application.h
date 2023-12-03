//
// Created by Admin on 2023/11/27.
//

#ifndef INC_3D_WARP_OPENGL_APPLICATION_H
#define INC_3D_WARP_OPENGL_APPLICATION_H

#include <filesystem>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <GLFW/glfw3.h>
#include "../warper/warper.h"
#include "../model_manager/model_manager.h"


class Application {
public:
    explicit Application(const std::filesystem::path& config_path);

    ~Application()=default;

    bool run();

private:
    Warper _warper;
    ModelManager _model_manager;
    Shader _shader;

    GLFWwindow *_window{};

    YAML::Node _config;

    float _far{1000},_near{10};
    // camera control
    float _last_x{}, _last_y{};
    bool _first_mouse{true};
    // timing
    float _delta_time = 0.0f;
    float _last_frame = 0.0f;

    bool init();

    void loadModels(const std::filesystem::path& path,int start_index_1, int end_index_1,int start_index_2, int end_index_2,int threads_num);

    void loadModel(const std::filesystem::path& path,const std::string& name);

    void updateCamera();

    void processInput(GLFWwindow *window);

    static void _framebuffer_size_callback(GLFWwindow* window, int width, int height);

    static void _mouse_callback(GLFWwindow* window, double xpos, double ypos);
};


#endif //INC_3D_WARP_OPENGL_APPLICATION_H
