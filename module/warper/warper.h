//
// Created by Admin on 2023/11/23.
//

#ifndef INC_3D_WARP_OPENGL_WARPER_H
#define INC_3D_WARP_OPENGL_WARPER_H

#include <string>
#include <Eigen/Dense>
#include <opencv2/core/mat.hpp>
#include "../camera/new_camera.h"
#include "../shader/shader.h"
#include "../model_manager/model_manager.h"

class Warper {
public:
    Warper()=default;

    /// width,height: viewport size
    Warper(unsigned int width,unsigned int height,const std::string& vertex_shader_path,const std::string& fragment_shader_path);
    ~Warper();

    void update(const NewCamera &camera_1, const NewCamera &camera_2);

    const cv::Mat & warp(ModelManager &model_manager, int threads_num);

    void setSize(unsigned int width, unsigned int height);

    void setShader(const std::string& vertex_shader_path,const std::string& fragment_shader_path){
        _projection_shader = Shader(vertex_shader_path, fragment_shader_path);
    }

    // viewport: x,y,width,height 视口回调需要调用
    void setViewport(Eigen::Vector4d viewport);

    [[nodiscard]] NewCamera& getCamera1(){return _camera_1;}
    [[nodiscard]] NewCamera& getCamera2(){return _camera_2;}

    [[nodiscard]] unsigned int getWidth() const{return _width;}
    [[nodiscard]] unsigned int getHeight() const{return _height;}

    [[nodiscard]] const Eigen::Vector4d& getViewport() const{return _viewport;}

private:
    unsigned int _width{},_height{};
    std::vector<GLfloat> _position_data;
    NewCamera _camera_1,_camera_2;
    Eigen::Vector4d _viewport;

    GLuint _framebuffer{},_attach_texture_{},_depth_buffer{};

    Shader _projection_shader;

    cv::Mat _musk;

    [[maybe_unused]] void ShowDepthMap();
};


#endif //INC_3D_WARP_OPENGL_WARPER_H
