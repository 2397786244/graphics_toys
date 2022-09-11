#define STB_IMAGE_IMPLEMENTATION
#include "Shader.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "iostream"
#include "Asset_Importer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
glm::vec3 CamPos(0.f,0.f,3.f);
glm::vec3 CamFront(0.f,0.f,-1.f);
glm::vec3 CamUp(0.f,1.f,0.f);
static float fov = 45.f;
static float lastTime = 0.f;
static float deltaTime = 0.f;
static float lastX = 400.f,lastY = 300.0f;  //记录鼠标上一帧的位置。计算方向向量用。
static float y_offset=0.f,x_offset=0.f;
static float sensitive = 0.02f;
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
    GLFWwindow * window = glfwCreateWindow(800,600,"GraphicsWindow", nullptr, nullptr);
    glfwMakeContextCurrent(window);  //设置窗口的上下文为当前线程的上下文。
    //glad初始化,加载opengl函数指针(管理opengl指针)
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
        std::cout << "Fail to load GLAD.\n";
        exit(0);
    }
    //告诉opengl视口大小。
    glViewport(0,0,800,600);
    //视口调整函数 注册。
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetScrollCallback(window,mouse_scroll);
    //程序隐藏光标并捕捉
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    stbi_set_flip_vertically_on_load(1);
    //读取纹理
    int width,height,channels;
    unsigned int grass_tex;
    unsigned char * data = stbi_load("../texture/grass.png",&width,&height,&channels,0);
    glGenTextures(1,&grass_tex);
    glBindTexture(GL_TEXTURE_2D,grass_tex);
    //设置纹理环绕方式、纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(data)
    {
        //用前面读取的纹理图像生成这个纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Load Texture Failed!\n");
    }
    stbi_image_free(data);

    //草着色器
    toy::Shader grassShader("../models/grass/grassVert.vert","../models/grass/grassFrag.frag");
    //创建草顶点缓冲区
    // grassPos TexCoords model
    //共三个面

    float grassData []=
             {
                     -0.5f, -0.5f,  0.f,  0.0f, 0.0f,
                     0.5f, -0.5f,  0.f,  1.0f, 0.0f,
                     0.5f,  0.5f,  0.f,  1.0f, 1.0f,
                     0.5f,  0.5f,  0.f,  1.0f, 1.0f,
                     -0.5f,  0.5f,  0.f,  0.0f, 1.0f,
                     -0.5f, -0.5f,  0.f,  0.0f, 0.0f,

                     -0.25 ,-0.5 ,0.43f, 0.0f, 0.0f,
                      0.25, -0.5, -0.43f,1.0f, 0.0f,
                      0.25, 0.5, -0.43f,1.0f, 1.0f,
                      0.25, 0.5 ,-0.43f,1.0f, 1.0f,
                      -0.25, 0.5, 0.43f, 0.0f, 1.0f,
                      -0.25, -0.5, 0.43f,  0.0f, 0.0f,

                      0.25, -0.5, 0.43,0.0f, 0.0f,
                      -0.25, -0.5 ,-0.43,1.0f, 0.0f,
                      -0.25 ,0.5, -0.43,1.0f, 1.0f,
                      -0.25, 0.5, -0.43,1.0f, 1.0f,
                      0.25 ,0.5, 0.43,0.0f, 1.0f,
                      0.25, -0.5, 0.43 ,0.0f, 0.0f
    };

    const int grassCount = 80;
    //生成随机的位置矩阵
    glm::mat4 originModel(1.0f);
    glm::mat4 model[grassCount];
    float r;
    float x,z;
    for(int i = 0;i<grassCount;i++)
    {
        r = (rand() % 14 + 2);  //2-15
        x = std::sin(glfwGetTime() + i) * r;
        z = std::cos(glfwGetTime() + i) * r;
        model[i] = glm::translate(originModel,glm::vec3(x,0.0,z));
    }

    //创建VAO、VBO。使用实例化数组
    unsigned int VAO;
    unsigned int grassVBO;
    unsigned int VBO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassData), &grassData[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenBuffers(1,&grassVBO);
    glBindBuffer(GL_ARRAY_BUFFER,grassVBO);
    glBufferData(GL_ARRAY_BUFFER, grassCount * sizeof(glm::mat4), &model[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    glm::mat4 view(1.0f);
    glm::mat4 projection(1.0f);

    glEnable(GL_DEPTH_TEST);
    grassShader.use();
    grassShader.send1i("grass",0);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.5,0.5,0.5,1.0f);  //状态设置
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //清空颜色缓冲位。 状态使用函数

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,grass_tex);

        view = cam.getLookAt();
        projection = cam.getPerspective();

        grassShader.use();
        grassShader.send1f("time",0.18f * std::sin(glfwGetTime()));
        grassShader.sendmat4fv("projection",projection);
        grassShader.sendmat4fv("view",view);

        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES,0,18,grassCount);

        glfwSwapBuffers(window);  //交换颜色缓冲。opengl使用双缓冲，前缓冲保存显示的图像缓冲，后缓冲区储存渲染指令正在绘制的。渲染指令
        //执行完毕后缓冲区交换。
        glfwPollEvents();  //检查触发事件。如键盘输入等。有则调用相应的函数进行更新。
    }
    glad_glDeleteProgram(grassShader.getShaderProgramID());
    glfwTerminate();
    return 0;
}


