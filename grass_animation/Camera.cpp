#include "Camera.h"
namespace toy{
    Camera::Camera(toy::frustum f, glm::vec3 campos, float yaw, float pitch, float sense,
                   float movespeed) {

    _fov = f.fov;
    _aspect = f.aspect;
    _zn = f.znear;
    _zf = f.zfar;
    _cpos = campos;
    _cfront = glm::vec3(0.f,0.f,-1.f);
    _wup = _cup = glm::vec3(0.f,1.f,0.f);
    _sense = sense;
    _mspeed = movespeed;
    _yaw = yaw;
    _pitch = pitch;
    }

    glm::mat4 Camera::getLookAt() {
return glm::lookAt(_cpos,_cpos+_cfront,_cup);
    }

    void Camera::CamMove(Camera::MoveDir dir,float deltatime) {
        if(dir == MoveDir::FRONT){
            _cpos += _cfront * _mspeed * deltatime;
        }
        else if (dir == MoveDir::BACK){
            _cpos -= _cfront * _mspeed * deltatime;
        }
        else if(dir == MoveDir::RIGHT){
            _cpos -= _mspeed * deltatime * glm::normalize(glm::cross(_cup,_cfront));
        }
        else if(dir == MoveDir::LEFT){
            _cpos += _mspeed * deltatime * glm::normalize(glm::cross(_cup,_cfront));
        }
    }

    void Camera::CamRotate(float x_offset, float y_offset) {
        _pitch += y_offset * _sense;
        _yaw += x_offset * _sense;
        if(_pitch > 80.f){
            _pitch = 80.f;
        }
        else if(_pitch < -80.f){
            _pitch = -80.f;
        }

        glm::vec3 newFront;
        newFront.x = std::cos(glm::radians(_pitch)) * std::sin(glm::radians(_yaw));
        newFront.y = std::sin(glm::radians(_pitch));
        newFront.z = std::cos(glm::radians(_pitch)) * -std::cos(glm::radians(_yaw));
        _cfront = glm::normalize(newFront);
    }
    void Camera::CamZoom(float yoffset) {
        _fov -= yoffset;
        if (_fov < 5.0f)
            _fov= 5.0f;
        if (_fov > 60.0f)
            _fov = 60.0f;
    }

    float Camera::getPitch() {
        return _pitch;
    }
    float Camera::getYaw() {
        return _yaw;
    }
    float Camera::getZoom() {
        return _fov;
    }
    glm::vec3 Camera::getCamPos() {
        return _cpos;
    }
    float Camera::setSpeed(float movespeed) {
        _mspeed = movespeed;
    }
    float Camera::setSense(float sense) {
        _sense = sense;
    }
    glm::mat4 Camera::getPerspective() {
        return glm::perspective(glm::radians(_fov),_aspect,_zn,_zf);
    }
    glm::vec3 Camera::getCamFront(){
        return _cfront;
    }
}