//
// Created by Admin on 2023/12/4.
//

#ifndef INC_3D_WARP_OPENGL_CAMERA_MANAGER_H
#define INC_3D_WARP_OPENGL_CAMERA_MANAGER_H

#include <vector>
#include <string>
#include <tinyxml2.h>
#include <glm/glm.hpp>
#include "../camera/new_camera.h"

enum CameraError {
    CAMERA_NO_ERROR,
    CAMERA_NO_MORE_CAMERA,
    CAMERA_NOT_FOUND_CORRESPONDING
};

class CameraManager {
public:
    CameraManager()=default;

    void init(const std::string& data_path, const std::string& name_path, const glm::vec3& origin_position);

    CameraError updateCamera(NewCamera &camera);

    void setUp(const glm::vec3& up){_up=up;}

    void setFront(const glm::vec3& front){_front=front;}

    void setNear(float near){_near=near;}

    void setFar(float far){_far=far;}

    void setCurrentCamera(int current_camera){_current_camera=current_camera;}

    [[nodiscard]] const glm::vec3& getOriginPosition() const{return _origin_position;}

    [[nodiscard]] const glm::vec3& getUp() const{return _up;}

    [[nodiscard]] const glm::vec3& getFront() const{return _front;}

    [[nodiscard]] const std::vector<std::string>& getCameraNames() const{return _camera_names;}

    [[nodiscard]] std::string getCurrentCameraName() const{return (_current_camera<_camera_names.size() && _current_camera>=0?_camera_names[_current_camera]:"");}

    [[nodiscard]] const tinyxml2::XMLDocument& getCameraData() const{return _camera_data;}

    [[nodiscard]] unsigned int getCameraNum() const{return _camera_names.size();}

    [[nodiscard]] int getCameraIndex() const{return _current_camera;}

    [[nodiscard]] int getCurrentCamera() const{return _current_camera;}

    [[nodiscard]] float getNear() const{return _near;}

    [[nodiscard]] float getFar() const{return _far;}

private:

    tinyxml2::XMLDocument _camera_data;
    std::vector<std::string> _camera_names;

    float _near{0.1},_far{1000};

    glm::vec3 _origin_position{},_up{0, -1, 0},_front{0, 0, 1};

    int _current_camera{-1};
};


#endif //INC_3D_WARP_OPENGL_CAMERA_MANAGER_H
