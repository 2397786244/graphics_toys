#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#include "iostream"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float initAlpha = 1.0f;
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
//窗口大小发生变化时，视口也会变化。
void framebuffer_size_callback(GLFWwindow*w,int height,int width){
    glViewport(0,0,height,width);
}

void mouse_callback(GLFWwindow*window,double xPos,double yPos){
    x_offset = sensitive * (xPos - lastX);
    y_offset = sensitive * (lastY - yPos);
    lastX=xPos;lastY=yPos;
    pitch += y_offset;
    yaw += x_offset;
    if(pitch > 80.f){
        pitch = 80.f;
    }
    else if(pitch < -80.f){
        pitch = -80.f;
    }

    glm::vec3 newFront;
    newFront.x = std::cos(glm::radians(pitch)) * std::sin(glm::radians(yaw));
    newFront.y = std::sin(glm::radians(pitch));
    newFront.z = std::cos(glm::radians(pitch)) * -std::cos(glm::radians(yaw));
    CamFront = glm::normalize(newFront);

}
//键盘输入处理
void processInput(GLFWwindow*window){
    static float speed = 2.0f;
    float curTime = glfwGetTime();
    deltaTime = curTime - lastTime;
    lastTime = curTime;
    if(glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS){
        if(fov < 80.f){
            fov += 0.1f;
        }
    }
    if(glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS){
        if(fov > 5.0f){
            fov -= 0.1f;
        }
    }
    if(glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS){
        //Campos的Z轴分量应该减小（即增加CamFront）
        CamPos += CamFront * speed * deltaTime;
    }
    if(glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS){
        CamPos -= CamFront * speed * deltaTime;
    }
    if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS){
        CamPos += speed * deltaTime * glm::normalize(glm::cross(CamUp,CamFront));
    }
    if(glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS){
        CamPos -= speed * deltaTime * glm::normalize(glm::cross(CamUp,CamFront));
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
        printf("Fail to load GLAD.\n");
        exit(0);
    }

    //告诉opengl视口大小。
    glViewport(0,0,800,600);

    //视口调整函数 注册。
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    //程序隐藏光标并捕捉
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //纹理图像读取
    int width,height,channels;
    stbi_set_flip_vertically_on_load(1);  //stb读取图像时反转，让原点位置在左下角。
    unsigned char *data= stbi_load("../wall.jpg",&width,&height,&channels,0);

    //创建纹理
    unsigned int texID;  //纹理id
    unsigned int wall_surf_tex;
    glGenTextures(1,&texID);
    //绑定纹理到texture2D，保证后面的设置都可以配置到当前的纹理上。
    glBindTexture(GL_TEXTURE_2D,texID);

    //设置纹理环绕方式、纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(data)
    {
        //用前面读取的纹理图像生成这个纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        /*
         * 第二个参数 设置多级渐远纹理的级别。这里0是基本级别。
         * 第三个参数 GL_RGB指明将纹理储存为什么格式。这里就是把纹理储存为RGB格式。
         * 第二个0的参数是历史遗留问题。都写0
         * 第七八个参数指定原图的格式和数据的类型。data为unsigned char类型
         * */
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Load Texture Failed!\n");
    }
    stbi_image_free(data);

    //读取墙面涂鸦纹理
    unsigned char * data2 = stbi_load("../pic.jpg",&width,&height,&channels,0);
    glGenTextures(1,&wall_surf_tex);
    glBindTexture(GL_TEXTURE_2D,wall_surf_tex);
    //设置纹理环绕方式、纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(data2)
    {
        //用前面读取的纹理图像生成这个纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        /*
         * 第二个参数 设置多级渐远纹理的级别。这里0是基本级别。
         * 第三个参数 GL_RGB指明将纹理储存为什么格式。这里就是把纹理储存为RGB格式。
         * 第二个0的参数是历史遗留问题。都写0
         * 第七八个参数指定原图的格式和数据的类型。data为unsigned char类型
         * */
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Load Texture Failed!\n");
    }
    stbi_image_free(data2);

    //顶点着色器GLSL代码
    const char * vertexShaderSource = "#version 460 core\n"
                                      "layout(location=0) in vec3 aPos;\n"
                                      "layout(location=1) in vec2 aTexCoord;\n"
                                      "layout(location=2) in vec3 aNormal;\n"
                                      "uniform mat4 projection;\n"
                                      "uniform mat4 view;\n"
                                      "uniform mat4 model;\n"
                                      "out vec2 TexCoord;"
                                      "out vec3 Normal;\n"
                                      "out vec3 FragPos;\n"
                                      "void main(){gl_Position=projection * view * model * vec4(aPos.xyz,1.0);FragPos = vec3(model * vec4(aPos.xyz,1.0));Normal = mat3(transpose(inverse(model)))*aNormal;TexCoord=aTexCoord; }";
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    //shader是否编译成功检查
    int success;
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
    if(!success){
        printf("Compile fail in vertexShader !\n");
        exit(0);
    }
    //片段着色器代码
    /*
#version 460 core
out vec4 FragColor;
     void main(){
FragColor = vec4(0.5,0.2,0.3,1.0);
     }
     */
    // 需要通过uniform变量传入的参数：
    // Texture1/2  纹理。不需要传入
    //ka ks 系数。 float类型
    //LightPos 光源位置  viewPos LightColor 光源颜色 均为vec3类型
    //specularParam  反光强度。即p。 float类型
    const char * fragmentShaderSource = "#version 460 core\n"
                                        "out vec4 FragColor;\n"
                                        "in vec2 TexCoord;"
                                        "in vec3 Normal;"
                                        "in vec3 FragPos;\n"
                                        "uniform sampler2D Texture1;\n"
                                        "uniform sampler2D Texture2;\n"
                                        "uniform float ka;\n"
                                        "uniform float ks;\n"
                                        "uniform vec3 LightPos;\n"
                                        "uniform vec3 viewPos;\n"
                                        "uniform vec3 LightColor;\n"
                                        "uniform float specularParam;\n"
                                        "void main(){\n"
                                        "vec4 kd = texture(Texture1,TexCoord);\n"
                                        "vec3 norm = normalize(Normal);\n"
                                        "vec3 light_vector = vec3(LightPos-FragPos);\n"
                                        "vec3 view_vector = normalize(viewPos-FragPos);\n"
                                        "float r = length(light_vector);"
                                        "light_vector = normalize(light_vector);\n"
                                        "vec3 ambient = ka * LightColor;\n"
                                        "float diff = max(0.0,dot(norm,light_vector));\n"
                                        "vec3 diffuse = diff * LightColor;\n"
                                        "float spec = pow(max(0.0,dot(view_vector,reflect(-light_vector,norm))),specularParam);\n"
                                        "vec3 specular = spec * ks * LightColor; \n"
                                        "FragColor = vec4(specular,1.0) + vec4(ambient + diffuse,1.0) * kd;\n"
                                        "}";


    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
    if(!success){
        printf("Compile fail in Fragment Shader!\n");
        exit(0);
    }
    //着色器程序链接
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    //链接是否成功的检查
    glad_glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
    if(!success){
        printf("LINK Fail!\n");
        exit(0);
    }
    //链接成功后，前面两个shader可以删掉
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //顶点数据初始化
    float vertexData[] = {
            // pos posY posZ //texcoordX Y    //Normal X   Y  Z
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f ,0.0f,  1.0f,  0.0f,
    };

    //定义立方体的位置
    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    //VAO  VBO 顶点属性 配置
    //一般物体的顶点属性等设置
    unsigned int VAO,VBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    //绑定VBO、配置属性
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertexData),vertexData,GL_STATIC_DRAW);

    // postionX posY posZ  S T  NormalX NormalY NormalZ postionX posY posZ S T Nx Ny Nz...
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8* sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(5*sizeof(float)));
    glEnableVertexAttribArray(2);  //各顶点法向量。
    //设置完成后解绑
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    /*
     * 光源设置。位置、光源物体顶点属性等。
     * */

    //光源位置
    glm::vec4 LightingPos = {0.f,0.f,0.f,1.0f};  //局部坐标系的位置
    glm::mat4 LightingModel(1.0f);
    //光照强度
    glm::vec3 LightingStrength = {1.0f,1.0f,1.0f};  //LightColor
    //发光源物体的顶点设置
    unsigned int LightVAO;  //VBO顶点数据使用前面的
    glGenVertexArrays(1,&LightVAO);
    glBindVertexArray(LightVAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    //光源的颜色固定,顶点位置从VBO获取。
    const char * LightingVertexShader = "#version 460 core\n"
                                        "layout(location=0)in vec3 aPos;\n"
                                        "uniform mat4 projection;\n"
                                        "uniform mat4 view;\n"
                                        "uniform mat4 model;\n"
                                        "void main(){\n"
                                        "gl_Position= projection * view * model * vec4(aPos.xyz,1.0f);\n"
                                        "}";

    const char * LightingFragmentShader = "#version 460 core\n"
                                          "out vec4 FragColor;\n"
                                          "void main(){\n"
                                          "FragColor = vec4(1.0f,1.0f,1.0f,1.0f);\n"
                                          "}";


    //模型变换、视图变换、投影变换矩阵并传递到顶点着色器
    glm::mat4 model(1.0f),projection(1.0f),view(1.0f);
    //view = glm::lookAt(glm::vec3(0.0f,0.0f,3.0f),glm::vec3(.0f,.0f,.0f),glm::vec3(.0f,1.f,.0f));

    //传递纹理单元到uniform类型的采样器变量中
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram,"Texture1"),0);
    glUniform1i(glGetUniformLocation(shaderProgram,"Texture2"),1);
    glUniform1f(glGetUniformLocation(shaderProgram,"alpha"),initAlpha);

    //传递这些参数ka ks 系数。 float类型
    //LightPos 光源位置  viewPos LightColor 光源颜色 均为vec3类型
    //specularParam  反光强度。即p。 float类型
    glUniform1f(glGetUniformLocation(shaderProgram,"ka"),0.20f);
    glUniform1f(glGetUniformLocation(shaderProgram,"ks"),0.85f);


    glUniform1f(glGetUniformLocation(shaderProgram,"specularParam"),128.0f);

    //开启深度测试
    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        glUniform1f(glGetUniformLocation(shaderProgram,"alpha"),initAlpha);
        glClearColor(0.2,0.3,0.3,1.0f);  //状态设置
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //清空颜色缓冲位。 状态使用函数

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,wall_surf_tex);
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);

        view = glm::lookAt(CamPos,CamPos + CamFront,CamUp);
        projection = glm::perspective(glm::radians(fov),4.0f/3.0f,0.1f,100.0f);
        //光源位置随时间变化。
        float posx = 15 * std::sin(glfwGetTime());
        float posz = 15 * std::cos(glfwGetTime());
        LightingModel = glm::translate(LightingModel,glm::vec3(posx,2.0f,posz));
        //颜色随时间变化
        float r = (1 + std::sin(glfwGetTime() * 0.5f))/2;
        float g = (1 + std::sin(glfwGetTime() * 0.8f))/2;
        float b = (1 + std::sin(glfwGetTime() * 0.2f))/2;
        glUniform3fv(glGetUniformLocation(shaderProgram,"LightColor"),1,glm::value_ptr(glm::vec3(r,g,b)));
        glUniform3fv(glGetUniformLocation(shaderProgram,"LightPos"),1,glm::value_ptr(glm::vec3(LightingModel * LightingPos)));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"view"),1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"projection"),1,GL_FALSE,glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(shaderProgram,"viewPos"),1,glm::value_ptr(CamPos));  //更新观察者位置（相机位置）

        for(unsigned int i = 0;i<10;i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model,cubePositions[i]);
            float angle = 15.0f * i;
            model = glm::rotate(model,glm::radians(angle),glm::vec3(0.5f,0.3f,0.7f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES,0,36);
        }

        glfwSwapBuffers(window);  //交换颜色缓冲。opengl使用双缓冲，前缓冲保存显示的图像缓冲，后缓冲区储存渲染指令正在绘制的。渲染指令
        //执行完毕后缓冲区交换。
        glfwPollEvents();  //检查触发事件。如键盘输入等。有则调用相应的函数进行更新。
    }
    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&VAO);
    glad_glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}



