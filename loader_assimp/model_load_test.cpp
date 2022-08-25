#define STB_IMAGE_IMPLEMENTATION
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Asset_Importer.h"
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 CamPos(0.f,0.f,3.f);
glm::vec3 CamFront(0.f,0.f,-1.f);
glm::vec3 CamUp(0.f,1.f,0.f);
static float fov = 45.f;
static float lastTime = 0.f;
static float deltaTime = 0.f;
static float lastX = 400.f,lastY = 300.0f;  //记录鼠标上一帧的位置。计算方向向量用。
static float y_offset=0.f,x_offset=0.f;
static float sensitive = 0.05f;
static float pitch=0.f,yaw=0.f;
static float aspect = 128.f/72.f;
toy::Camera cam(toy::frustum {fov,aspect,0.1f,100.0f},CamPos,yaw,pitch,sensitive);
//窗口大小发生变化时，视口也会变化。
void framebuffer_size_callback(GLFWwindow*w,int height,int width){
    glViewport(0,0,height,width);
}
//鼠标滚轮处理
void mouse_scroll(GLFWwindow*w,double xoffset,double yoffset){
    cam.CamZoom(float(yoffset));
}

void mouse_callback(GLFWwindow*window,double xPos,double yPos){
    x_offset = xPos - lastX;
    y_offset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;
    cam.CamRotate(x_offset,y_offset);
}
//键盘输入处理
void processInput(GLFWwindow*window){
    float curTime = glfwGetTime();
    deltaTime = curTime - lastTime;
    lastTime = curTime;
    if(glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS){
        cam.CamMove(toy::Camera::MoveDir::FRONT,deltaTime);
    }
    if(glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS){
        cam.CamMove(toy::Camera::MoveDir::BACK,deltaTime);
    }
    if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS){
        cam.CamMove(toy::Camera::MoveDir::LEFT,deltaTime);
    }
    if(glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS){
        cam.CamMove(toy::Camera::MoveDir::RIGHT,deltaTime);
    }
}

int main(){
    glfwInit();
    glfwInitHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwInitHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwInitHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    //GLFW初始化。

    GLFWwindow * window = glfwCreateWindow(1280,720,"GraphicsWindow", nullptr, nullptr);
    glfwMakeContextCurrent(window);  //设置窗口的上下文为当前线程的上下文。

    //glad初始化,加载opengl函数指针(管理opengl指针)
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
        printf("Fail to load GLAD.\n");
        exit(0);
    }

    //告诉opengl视口大小。
    glViewport(0,0,1280,720);

    //视口调整函数 注册。
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetScrollCallback(window,mouse_scroll);
    //程序隐藏光标并捕捉
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    //模型载入
    toy::Model rockModel("../models/rock/cube.obj");
    //着色器初始化
    toy::Shader rockShader("../models/rock/rockVertShader.vert","../models/rock/rockFragShader.frag");
    rockShader.use();
    glm::mat4 model(1.0f);
    model = glm::translate(model,glm::vec3(0.f,0.f,0.f));
    model = glm::scale(model,glm::vec3(0.5f,0.5f,0.5f));
    rockShader.sendmat4fv("model",model);
    rockShader.send3fv("LightPos",glm::vec3(0,0,1.5f));
    rockShader.send3fv("LightColor",{1.f,1.f,1.f});
    rockShader.send1f("specularParam",128.f);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.1f,0.1f,0.1f,1.f);  //状态设置
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //清空颜色缓冲位。 状态使用函数
        rockShader.use();
        rockShader.sendmat4fv("view",cam.getLookAt());
        rockShader.sendmat4fv("projection",cam.getPerspective());
        rockShader.send3fv("viewPos",cam.getCamPos());

        //传递MVP数组、片段着色器所需变量。
        rockModel.Draw(rockShader);
        glfwSwapBuffers(window);  //交换颜色缓冲。opengl使用双缓冲，前缓冲保存显示的图像缓冲，后缓冲区储存渲染指令正在绘制的。渲染指令
        //执行完毕后缓冲区交换。
        glfwPollEvents();  //检查触发事件。如键盘输入等。有则调用相应的函数进行更新。
    }

    glfwTerminate();
    return 0;
}