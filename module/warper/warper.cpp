//
// Created by Admin on 2023/11/23.
//

#include <glad/glad.h>
#include "warper.h"
#include <vector>
#include <thread>
#include <opencv2/opencv.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_bool(flip_depth_map, false, "flip depth map vertically");
DEFINE_bool(debug, false, "debug mode");

using namespace std;

Warper::Warper(const unsigned int width, const unsigned int height,const std::string& vertex_shader_path,
               const std::string& fragment_shader_path): _width(width), _height(height),
                                                         _projection_shader(vertex_shader_path, fragment_shader_path),
                                                         _position_data(width * height * 3),
                                                         _musk(_height,_width,CV_8UC1,cv::Scalar(0)){
    _viewport = Eigen::Vector4d(0,0,width,height);

    // create a framebuffer object
    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // create a texture object
    glGenTextures(1, &_attach_texture_);
    glBindTexture(GL_TEXTURE_2D, _attach_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _width, _height, 0,GL_RGB , GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _attach_texture_, 0);

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &_depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depth_buffer); // now actually attach it

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG(ERROR) << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Warper::update(const NewCamera &camera_1, const NewCamera &camera_2) {
    _camera_1 = camera_1;
    _camera_2 = camera_2;
}

const cv::Mat & Warper::warp(ModelManager &model_manager, int threads_num) {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // 清除颜色缓冲和深度缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projectoin_1(_camera_1.getProjectionMatrix()), projectoin_2(_camera_2.getProjectionMatrix()),
            view_1(_camera_1.getViewMatrix()), view_2(_camera_2.getViewMatrix());
    glm::vec4 viewport(0,0,_width,_height);

    _projection_shader.use();
    _projection_shader.setMat4("projection_1", projectoin_1);
    _projection_shader.setMat4("projection_2", projectoin_2);
    _projection_shader.setMat4("view_1", view_1);
    _projection_shader.setMat4("view_2", view_2);
    _projection_shader.setVec4("viewport", viewport);
    model_manager.drawAllModel(_projection_shader);

    // 读取坐标缓冲
    glReadPixels(0, 0, (int)_width, (int)_height, GL_RGB, GL_FLOAT, _position_data.data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _musk=cv::Mat::zeros(_height,_width,CV_8UC1);
    vector<thread> threads;
    mutex mutex;
    // 多线程画点
    for(int i(0);i<threads_num;++i) {
        threads.emplace_back([&,i]() {
            // 新建局部变量，防止多线程冲突
            cv::Mat image_local(_height, _width, CV_8UC1, cv::Scalar(0));
            for (int j(i); j < _position_data.size()/3; j += threads_num) {
                // 将周围的点也画出来
                int x = int (_position_data[j*3]*_width), y = int(_height - _position_data[j*3+1]*_height);
                for (int k(-1); k <= 1; k++) {
                    for (int l(-1); l <= 1; l++) {
                        int new_x(x+l),new_y(y+k);
                        if (new_x < 0 || new_x >= _width || new_y < 0 || new_y >= _height) {
                            continue;
                        }
                        image_local.at<uchar>(new_y, new_x) = uchar(255);
                    }
                }
            }
            // 上线程锁
            std::lock_guard<std::mutex> lockGuard(mutex);
            cv::add(_musk,image_local,_musk);
        });
    }
    for (auto &thread: threads) {
        thread.join();
    }

    // 单线程版本
//        for (int i(0); i < _position_data.size() / 3; ++i) {
//            // 将周围的点也画出来
//            int x=int(_position_data[i * 3]*(float)_width),y=int((float)_height-_position_data[i * 3 + 1]*(float)_height);
//            for(int k(-1);k<=1;k++){
//                for(int l(-1);l<=1;l++){
//                    if(x+l<0||x+l>=_width||y+k<0||y+k>=_height){
//                        continue;
//                    }
//                    image.at<uchar>(y+k,x+l)=uchar(255);
//                }
//            }
//        }
    if(FLAGS_debug){
        cv::imshow("camera_2_musk",_musk);
        cv::waitKey(1);
    }

    return _musk;
}

void Warper::ShowDepthMap() {
    cv::Mat depthImage(_height, _width, CV_32FC1);

// 将深度值映射到0到1之间
    cv::normalize(depthImage, depthImage, 0, 1, cv::NORM_MINMAX);

    // 将深度图上下反转
    if(FLAGS_flip_depth_map){
        cv::flip(depthImage, depthImage, 0);
    }

// 可选：将深度图转换为8位图像以显示
    cv::Mat depthImage8U;
    depthImage.convertTo(depthImage8U, CV_8U, 255);

// 显示深度图
    cv::imshow("Depth Image", depthImage8U);
    cv::waitKey(1);
}

void Warper::setSize(const unsigned int width, const unsigned int height) {
    this->_width = width;
    this->_height = height;
    _position_data = vector<GLfloat>(width * height * 3);
    _viewport = Eigen::Vector4d(0,0,_width,_height);
    _musk=cv::Mat(_height,_width,CV_8UC1,cv::Scalar(0));

    // 在framebuffer中修改纹理
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // 改变纹理大小
    glBindTexture(GL_TEXTURE_2D, _attach_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0,GL_RGB , GL_FLOAT, nullptr);
    // 改变深度缓冲大小
    glBindRenderbuffer(GL_RENDERBUFFER, _depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Warper::setViewport(Eigen::Vector4d viewport) {
    _viewport=viewport;
    this->_width = viewport(2);
    this->_height = viewport(3);
    _position_data = vector<GLfloat>(_width * _height * 3);
    _musk=cv::Mat(_height,_width,CV_8UC1,cv::Scalar(0));

    // 在framebuffer中修改纹理
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // 改变纹理大小
    glBindTexture(GL_TEXTURE_2D, _attach_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0,GL_RGB , GL_FLOAT, nullptr);
    // 改变深度缓冲大小
    glBindRenderbuffer(GL_RENDERBUFFER, _depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Warper::~Warper() {
    glDeleteFramebuffers(1,&_framebuffer);
    glDeleteTextures(1,&_attach_texture_);
    glDeleteRenderbuffers(1,&_depth_buffer);
}





