//
// Created by Admin on 2023/11/27.
//

#include "new_camera.h"

#include <glm/gtc/matrix_transform.hpp>

NewCamera::NewCamera(const glm::vec3 &position, const glm::vec3 &front, const glm::vec3 &up, const glm::mat3 &rotation,
                     float fov, float aspect, float far, float near):_position(position),_origin_front(front),_origin_up(up)
                     ,_fov(fov),_aspect(aspect),_far(far),_near(near) {
    _front=_origin_front;
    _up=_origin_up;
    _origin_right=glm::cross(_origin_front,_origin_up);
    _right=glm::cross(_origin_front,_origin_up);
    rotate(rotation);
}

NewCamera::NewCamera(const glm::vec3 &position, const glm::vec3 &front, const glm::vec3 &up, float yaw, float pitch,
                     float fov, float aspect, float far, float near):_position(position),_origin_front(front),_origin_up(up),
                     _rotation(glm::mat3(1)),_fov(fov),_aspect(aspect),_far(far),_near(near) {
    _front=_origin_front;
    _up=_origin_up;
    _origin_right=glm::cross(_origin_front,_origin_up);
    _right=glm::cross(_origin_front,_origin_up);
    rotate(_up,yaw);
    rotate(_right,pitch);
}

glm::mat4 NewCamera::getViewMatrix() const {
    return glm::lookAt(_position, _position + _front, _up);
}

glm::mat4 NewCamera::getProjectionMatrix() const {
    return glm::perspective(_fov,_aspect,_near,_far);
}

void NewCamera::rotate(const glm::mat3 &rotation) {
    _rotation=rotation*_rotation;
    _front=_rotation*_origin_front;
    _up=_rotation*_origin_up;
    _right=glm::cross(_front,_up);
}

void NewCamera::rotate(const glm::vec3 &axis, float angle) {
    rotate(glm::mat3(glm::rotate(glm::mat4(1),angle,axis)));
}

void NewCamera::translate(const glm::vec3 &translation) {
    _position+=translation;
}

void NewCamera::processKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = _movement_speed * deltaTime;
    float velocity_roll = _roll_sensitivity * deltaTime;
    if (direction == FORWARD)
        _position += _front * velocity;
    else if (direction == BACKWARD)
        _position -= _front * velocity;
    else if (direction == LEFT)
        _position -= _right * velocity;
    else if (direction == RIGHT)
        _position += _right * velocity;
    else if (direction == ROTATE_LEFT)
        rotate(_front,-velocity_roll);
    else if (direction == ROTATE_RIGHT)
        rotate(_front,velocity_roll);
}

void NewCamera::processMouseMovement(float x_offset, float y_offset) {
    x_offset *= _mouse_sensitivity;
    y_offset *= _mouse_sensitivity;

    rotate(_origin_up,x_offset);
    rotate(_right,y_offset);
}

void NewCamera::setOptions(float movement_speed, float mouse_sensitivity, float roll_sensitivity) {
    _movement_speed=movement_speed;
    _mouse_sensitivity=mouse_sensitivity;
    _roll_sensitivity=roll_sensitivity;
}

const glm::vec3 &NewCamera::getPosition() const {
    return _position;
}

const glm::vec3 &NewCamera::getFront() const {
    return _front;
}

const glm::vec3 &NewCamera::getUp() const {
    return _up;
}

const glm::vec3 &NewCamera::getRight() const {
    return _right;
}

const glm::mat3 &NewCamera::getRotationMatrix() const {
    return _rotation;
}

float NewCamera::getMovementSpeed() const {
    return _movement_speed;
}

float NewCamera::getMouseSensitivity() const {
    return _mouse_sensitivity;
}

float NewCamera::getFov() const {
    return _fov;
}




