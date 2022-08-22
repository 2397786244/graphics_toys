#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#include "iostream"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Shader.h"

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
    //纹理图像读取
    int width,height,channels;
    stbi_set_flip_vertically_on_load(1);  //stb读取图像时反转，让原点位置在左下角。
    unsigned char *data= stbi_load("../container2.jpg",&width,&height,&channels,0);

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
    unsigned char * data2 = stbi_load("../container2_specular.jpg",&width,&height,&channels,0);
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

    //创建Shader类。加载着色器代码
    toy::Shader shader("../vertexShader.vert","../phong.frag");

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
    glm::vec4 LightingPos = {1.0f,0.0f,1.0f,1.0f};  //局部坐标系的位置
    glm::mat4 LightingModel(1.0f);
    //光照强度
    glm::vec3 LightingStrength = {0.0f,1.0f,0.0f};  //LightColor
    //发光源物体的顶点设置

    unsigned int LightVAO;  //VBO顶点数据使用前面的
    glGenVertexArrays(1,&LightVAO);
    glBindVertexArray(LightVAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    //光源的颜色固定,顶点位置从VBO获取。
    const char * LightingVertexShader = "#version 460 core\n"
                                        "layout(location=0)in vec3 aPos;\n"
                                        "uniform mat4 projection;\n"
                                        "uniform mat4 view;\n"
                                        "uniform mat4 model;\n"
                                        "void main(){\n"
                                        "gl_Position = projection * view * model * vec4(aPos.xyz,1.0f);\n"
                                        "}";

    const char * LightingFragmentShader = "#version 460 core\n"
                                          "out vec4 FragColor;\n"
                                          "void main(){\n"
                                          "FragColor = vec4(0.0f,0.8f,0.0f,1.0f);\n"
                                          "}";

    int success;
    //发光物体 着色器编译
    unsigned int light_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(light_vertexShader,1,&LightingVertexShader, nullptr);
    glCompileShader(light_vertexShader);
    //shader是否编译成功检查
    glGetShaderiv(light_vertexShader,GL_COMPILE_STATUS,&success);
    if(!success){
        printf("Compile fail in vertexShader !\n");
        exit(0);
    }

    unsigned int light_fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(light_fragShader,1,&LightingFragmentShader, nullptr);
    glCompileShader(light_fragShader);
    //shader是否编译成功检查
    glGetShaderiv(light_fragShader,GL_COMPILE_STATUS,&success);
    if(!success){
        printf("Compile fail in FragmentShader !\n");
        exit(0);
    }
    //光源物体 着色器程序 链接
    unsigned int light_ShaderProgram;
    light_ShaderProgram = glCreateProgram();
    glAttachShader(light_ShaderProgram,light_vertexShader);
    glAttachShader(light_ShaderProgram,light_fragShader);
    glLinkProgram(light_ShaderProgram);

    glDeleteShader(light_vertexShader);
    glDeleteShader(light_fragShader);
    glad_glGetProgramiv(light_ShaderProgram,GL_LINK_STATUS,&success);
    if(!success){
        printf("LINK Fail!\n");
        exit(0);
    }

    glm::vec3 Light_Pos[] = {
            glm::vec3(LightingPos),
            glm::vec3(-2.0f,1.0f,1.0f),
            glm::vec3(3.0f,1.5f,-4.0f),
            glm::vec3(-2.0f,2.0f,-8.0f)
    };
    glm::vec3 LightBase_Pos[] = {
            glm::vec3(LightingPos),
            glm::vec3(-2.0f,1.0f,1.0f),
            glm::vec3(3.0f,1.5f,-4.0f),
            glm::vec3(-2.0f,2.0f,-8.0f)
    };

    //模型变换、视图变换、投影变换矩阵并传递到顶点着色器
    glm::mat4 model(1.0f),projection(1.0f),view(1.0f);

    //传递纹理单元到uniform类型的采样器变量中
    shader.use();
    shader.send1i("Texture1",0);
    shader.send1i("Texture2",1);

    //传递这些参数ka ks 系数。 float类型
    //LightPos 光源位置  viewPos LightColor 光源颜色 均为vec3类型
    //specularParam  反光强度。即p。 float类型
    shader.send1f("ka",0.03f);
    shader.send1f("specularParam",128.0f);
    shader.send3fv("LightColor",glm::vec3(0.f,1.f,0.f));
    shader.send3fv("SpotColor",glm::vec3(1.f,1.f,1.f));
    shader.send3fv("spotDir",cam.getCamFront());
    shader.send1f("cutOff",std::cos(glm::radians(15.f)));
    shader.send1f("outerCircle",std::cos(glm::radians(25.f)));
    //开启深度测试
    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.1f,0.1f,0.1f,1.f);  //状态设置
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //清空颜色缓冲位。 状态使用函数

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,wall_surf_tex);
        //绘制光源物体。着色器为light_ShaderProgram
        glUseProgram(light_ShaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(light_ShaderProgram,"view"),1,GL_FALSE,glm::value_ptr(cam.getLookAt()));

        glUniformMatrix4fv(glGetUniformLocation(light_ShaderProgram,"projection"),1,GL_FALSE,glm::value_ptr(cam.getPerspective()));

        glBindVertexArray(LightVAO);
        //点光源位置随时间变化.
        glm::mat4 move(1.0f);
        float tX= 2 * std::sin(glfwGetTime());
        float tZ=2 * std::cos(glfwGetTime());
        move = glm::translate(move,glm::vec3(tX,0.f,tZ));
        for(int i = 0;i<4;i++)
        {
            Light_Pos[i] = glm::vec3(move * glm::vec4(LightBase_Pos[i],1.f));
            model = glm::mat4(1.0f);
            model = glm::translate(model, Light_Pos[i]);
            model = glm::scale(model,glm::vec3(0.2f));
            glUniformMatrix4fv(glGetUniformLocation(light_ShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        shader.use();
        glBindVertexArray(VAO);
        shader.sendmat4fv("view",cam.getLookAt());
        shader.sendmat4fv("projection",cam.getPerspective());
        shader.send3fv("viewPos",cam.getCamPos());
        shader.send3fv("spotDir",cam.getCamFront());
        //传递所有的点光源位置给fragmentShader中的LightPos数组。
        shader.send3fv("LightPos[0]",Light_Pos[0]);
        shader.send3fv("LightPos[1]",Light_Pos[1]);
        shader.send3fv("LightPos[2]",Light_Pos[2]);
        shader.send3fv("LightPos[3]",Light_Pos[3]);
        //绘制所有盒子
        for(unsigned int i = 0;i<10;i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model,cubePositions[i]);
            float angle = 15.0f * i;
            model = glm::rotate(model,glm::radians(angle),glm::vec3(0.5f,0.3f,0.7f));
            shader.sendmat4fv("model",model);
            glDrawArrays(GL_TRIANGLES,0,36);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);  //交换颜色缓冲。opengl使用双缓冲，前缓冲保存显示的图像缓冲，后缓冲区储存渲染指令正在绘制的。渲染指令
        //执行完毕后缓冲区交换。
        glfwPollEvents();  //检查触发事件。如键盘输入等。有则调用相应的函数进行更新。
    }
    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&VAO);
    glad_glDeleteProgram(shader.getShaderProgramID());
    glfwTerminate();
    return 0;
}



