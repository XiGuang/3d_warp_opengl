//
// Created by Admin on 2023/11/27.
//

#ifndef INC_3D_WARP_OPENGL_NEW_CAMERA_H
#define INC_3D_WARP_OPENGL_NEW_CAMERA_H

#include <glm/glm.hpp>
#include <cmath>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    ROTATE_LEFT,
    ROTATE_RIGHT,
};

// Default camera values
const float SPEED       =  5.0f;
const float SENSITIVITY =  0.001f;
const float ROLL_SENSITIVITY = 1.0f;

class NewCamera {
public:
    NewCamera(const glm::vec3 &position, const glm::vec3 &front, const glm::vec3 &up,const glm::mat3& rotation,
              float fov=M_PI_2,float aspect=1,float far=1000,float near=0.1);

    explicit NewCamera(const glm::vec3 &position={0,0,0}, const glm::vec3 &front={0,0,1}, const glm::vec3 &up={0,1,0},
              float yaw=0, float pitch=0,float fov=M_PI_2,float aspect=1,float far=1000,float near=0.1);

    void setOptions(float movement_speed, float mouse_sensitivity, float roll_sensitivity);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    [[nodiscard]] glm::mat4 getViewMatrix() const;

    [[nodiscard]] glm::mat4 getProjectionMatrix() const;

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(Camera_Movement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float x_offset, float y_offset);

    void rotate(const glm::mat3 &rotation);

    void rotate(const glm::vec3 &axis, float angle);

    void translate(const glm::vec3 &translation);

    [[nodiscard]] const glm::vec3 &getPosition() const;

    [[nodiscard]] const glm::vec3 &getFront() const;

    [[nodiscard]] const glm::vec3 &getUp() const;

    [[nodiscard]] const glm::vec3 &getRight() const;

    [[nodiscard]] const glm::mat3 &getRotationMatrix() const;

    [[nodiscard]] float getMovementSpeed() const;

    [[nodiscard]] float getMouseSensitivity() const;

    [[nodiscard]] float getFov() const;

    [[nodiscard]] float getNear() const{return _near;}

    [[nodiscard]] float getFar() const{return  _far;}

private:
    // camera Attributes
    glm::vec3 _position{};
    glm::vec3 _front;
    glm::vec3 _up{};
    glm::vec3 _right{};
    glm::vec3 _origin_front;
    glm::vec3 _origin_up;
    glm::vec3 _origin_right;
    glm::mat3 _rotation = glm::mat3(1.0f);  // 存储旋转矩阵

    float _yaw{0},_pitch{0},_roll{0};

    // 投影参数
    float _fov;
    float _near;
    float _far;
    float _aspect;

    // 畸变矩阵参数
    float _k1;
    float _k2;
    float _k3;
    float _p1;
    float _p2;

    // camera options
    float _movement_speed{SPEED};
    float _mouse_sensitivity{SENSITIVITY};
    float _roll_sensitivity{ROLL_SENSITIVITY};
};


#endif //INC_3D_WARP_OPENGL_NEW_CAMERA_H
