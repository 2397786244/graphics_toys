#ifndef OPENGL_TEST_CAMERA_H
#define OPENGL_TEST_CAMERA_H
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace toy {
    struct frustum{
        float fov;
        float aspect;
        float znear;
        float zfar;
    };
    class Camera {
    public:
        enum MoveDir{RIGHT,LEFT,FRONT,BACK};
        Camera(frustum f,glm::vec3 campos,float yaw = 0.0f,float pitch=0.0f,float sense=0.05f,float movespeed = 3.0f);
        glm::mat4 getLookAt();
        float getPitch();
        float getYaw();
        float getZoom();
        glm::vec3 getCamPos();
        float setSpeed(float movespeed);
        float setSense(float sense);
        void CamMove(MoveDir dir,float deltatime);
        void CamZoom(float yoffset);
        void CamRotate(float x_offset,float y_offset);
        glm::mat4 getPerspective();
        glm::vec3 getCamFront();
    private:

        float _fov;
        float _aspect;
        float _zn;
        float _zf;
        glm::vec3 _cpos;
        glm::vec3 _cfront;
        glm::vec3 _cup;
        float _pitch;
        float _yaw;
        float _sense;
        float _mspeed;
        glm::vec3 _wup;  //世界坐标系中的上方向。

    };

}

#endif //OPENGL_TEST_CAMERA_H
