#ifndef OPENGL_TEST_SHADER_H
#define OPENGL_TEST_SHADER_H
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace toy{
    class Shader{
    public:
         Shader(const char * vertexShaderPath,const char * fragShaderPath,const char * geoShaderPath= nullptr);
         void use() const;  //执行着色器程序
         void send1f(const char * uname,float _1f);
         void send3fv(const char * uname,glm::vec3 _3fv);
         void sendmat4fv(const char * uname,glm::mat4 _mat4fv);
         void send1i(const char * uname,int _1i);
         void send2fv(const char * uname,glm::vec2 _2fv);
         unsigned int getShaderProgramID();
    private:
        unsigned int _shaderProgram;
    };
}
#endif //OPENGL_TEST_SHADER_H
