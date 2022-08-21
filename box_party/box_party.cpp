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
    //ka 系数。 float类型
    //LightPos 光源位置  viewPos LightColor 光源颜色 均为vec3类型
    //specularParam  反光强度。即p。 float类型
    const char * fragmentShaderSource = "#version 460 core\n"
                                        "#define LIGHT_POS 4\n"
                                        "out vec4 FragColor;\n"
                                        "in vec2 TexCoord;\n"
                                        "in vec3 Normal;\n"
                                        "in vec3 FragPos;  //片段位置\n"
                                        "uniform sampler2D Texture1;  //材质纹理\n"
                                        "uniform sampler2D Texture2;  //高光纹理\n"
                                        "uniform float ka;  //环境光系数\n"
                                        "uniform vec3 LightPos[LIGHT_POS];   //点光源位置\n"
                                        "uniform vec3 viewPos;  //观察位置 或聚光灯位置\n"
                                        "uniform vec3 LightColor;  //点光源颜色\n"
                                        "uniform vec3 SpotColor;\n"
                                        "uniform float cutOff;  //聚光灯内圆锥范围  单位弧度制\n"
                                        "uniform vec3 spotDir;  //聚光灯朝向。即摄像机正面朝向。\n"
                                        "uniform float specularParam;  //光泽度\n"
                                        "uniform float outerCircle;  //聚光灯外圆锥角度。 弧度制\n"
                                        "\n"
                                        "//返回点光源着色结果\n"
                                        "vec3 calcPointLight(vec3 LightPos){\n"
                                        "    vec3 common_ambient = ka * LightColor;  //通用的环境光计算\n"
                                        "    vec3 kd = vec3(texture(Texture1,TexCoord));\n"
                                        "    vec3 norm = normalize(Normal);\n"
                                        "    vec3 view_vec = normalize(viewPos-FragPos);  //相机观察向量。注意这里标准化后的\n"
                                        "    vec3 dot_light_vec = vec3(LightPos-FragPos);\n"
                                        "    float dot_ray_r = length(dot_light_vec);\n"
                                        "    float dot_F = 1.0 / (1.0 + dot_ray_r * 0.22 + pow(dot_ray_r,2) * 0.2);\n"
                                        "    dot_light_vec = normalize(dot_light_vec);    \n"
                                        "    float dot_diff = max(0.0,dot(norm,dot_light_vec));\n"
                                        "    vec3 dot_diffuse = dot_diff * LightColor;\n"
                                        "    float dot_spec = pow(max(0.0,dot(view_vec,reflect(-dot_light_vec,norm))),specularParam);\n"
                                        "    vec3 dot_specular = dot_spec * vec3(texture(Texture2,TexCoord)) * LightColor; \n"
                                        "    vec3 Dot_Light_Color = (common_ambient + dot_diffuse * dot_F)* kd + dot_specular * dot_F;\n"
                                        "    return Dot_Light_Color;\n"
                                        "}\n"
                                        "\n"
                                        "//返回聚光灯着色结果\n"
                                        "vec3 calcSpotLight(){\n"
                                        "    vec3 kd = vec3(texture(Texture1,TexCoord));\n"
                                        "    vec3 norm = normalize(Normal);\n"
                                        "    vec3 spot_light_vec = vec3(viewPos-FragPos);  //聚光灯光线向量.\n"
                                        "    vec3 view_vec = normalize(viewPos-FragPos);  //相机观察向量。注意这里标准化后的\n"
                                        "    float spot_ray_r = length(spot_light_vec);\n"
                                        "    float spot_F = 1.0 / (1.0 + spot_ray_r * 0.045 + pow(spot_ray_r,2) * 0.0075);\n"
                                        "    spot_light_vec = normalize(spot_light_vec);\n"
                                        "    float spot_diff =  max(0.0,dot(norm,spot_light_vec));\n"
                                        "    vec3 spot_diffuse = spot_diff * SpotColor;\n"
                                        "    float spot_spec = pow(max(0.0,dot(view_vec,reflect(-spot_light_vec,norm))),specularParam);\n"
                                        "    vec3 spot_specular = spot_spec * vec3(texture(Texture2,TexCoord)) * SpotColor;\n"
                                        "    vec3 Spot_Light_Color = spot_specular * spot_F + spot_diffuse* kd * spot_F ; \n"
                                        "    return Spot_Light_Color;\n"
                                        "}\n"
                                        "\n"
                                        "\n"
                                        "void main(){\n"
                                        "    float theta = dot(normalize(FragPos-viewPos),normalize(spotDir));\n"
                                        "    //光线与聚光灯方向夹角。由此判断片段是否在聚光灯可照范围内。\n"
                                        "    //在内圆锥内，最终颜色为 聚光灯光线造成的高光、漫反射 + 环境光。\n"
                                        "    //如果在外圆锥到内圆锥区间内，插值计算得 inCircle，最终颜色为 聚光灯光线 * inCircle + 点光源的光线\n"
                                        "    //外圆锥之外的范围，最终颜色为 点光源光线计算后的颜色。     \n"
                                        "    float inCircle = (theta-outerCircle)/(cutOff-outerCircle);\n"
                                        "    vec3 result;\n"
                                        "    vec3 SpotLight = calcSpotLight();\n"
                                        "    //循环遍历点光源数组，计算对于这个片段的最终颜色。根据theta角度判断是否要计算聚光灯颜色。\n"
                                        "    for(int i = 0;i<LIGHT_POS;i++)\n"
                                        "    {\n"
                                        "        result += calcPointLight(LightPos[i]);\n"
                                        "    }\n"
                                        "\n"
                                        "    if(theta>=cutOff)\n"
                                        "    {\n"
                                        "        FragColor = vec4(result + SpotLight,1.0); //加上聚光灯。\n"
                                        "    }\n"
                                        "    else if(theta >= outerCircle)\n"
                                        "    {\n"
                                        "        //在内圆锥和外圆锥中间。 聚光灯部分光线 + 点光源\n"
                                        "        FragColor = vec4(SpotLight * inCircle + result,1.0);\n"
                                        "    }\n"
                                        "    else\n"
                                        "    {\n"
                                        "        FragColor = vec4(result,1.0);\n"
                                        "    }\n"
                                        "\n"
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
                                          "FragColor = vec4(1.0f,1.0f,1.0f,1.0f);\n"
                                          "}";

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

    //模型变换、视图变换、投影变换矩阵并传递到顶点着色器
    glm::mat4 model(1.0f),projection(1.0f),view(1.0f);

    //传递纹理单元到uniform类型的采样器变量中
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram,"Texture1"),0);
    glUniform1i(glGetUniformLocation(shaderProgram,"Texture2"),1);
    glUniform1f(glGetUniformLocation(shaderProgram,"alpha"),initAlpha);

    //传递这些参数ka ks 系数。 float类型
    //LightPos 光源位置  viewPos LightColor 光源颜色 均为vec3类型
    //specularParam  反光强度。即p。 float类型
    glUniform1f(glGetUniformLocation(shaderProgram,"ka"),0.03f);

    glUniform1f(glGetUniformLocation(shaderProgram,"specularParam"),128.0f);
    glUniform3fv(glGetUniformLocation(shaderProgram,"LightColor"),1,glm::value_ptr(glm::vec3(0.f,1.f,0.f)));
    glUniform3fv(glGetUniformLocation(shaderProgram,"SpotColor"),1,glm::value_ptr(glm::vec3(1.f,1.f,1.f)));
    glUniform3fv(glGetUniformLocation(shaderProgram,"spotDir"),1,glm::value_ptr(CamFront));
    glUniform1f(glGetUniformLocation(shaderProgram,"cutOff"),std::cos(glm::radians(15.f)));
    glUniform1f(glGetUniformLocation(shaderProgram,"outerCircle"),std::cos(glm::radians(25.f)));
    //开启深度测试
    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.5f,0.5f,0.5f,1.f);  //状态设置
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //清空颜色缓冲位。 状态使用函数

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,wall_surf_tex);
        //变换矩阵更新。
        view = glm::lookAt(CamPos,CamPos + CamFront,CamUp);
        projection = glm::perspective(glm::radians(fov),128.0f/72.0f,1.f,100.0f);
        //绘制光源物体。着色器为light_ShaderProgram
        /*glUseProgram(light_ShaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(light_ShaderProgram,"view"),1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(light_ShaderProgram,"projection"),1,GL_FALSE,glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(light_ShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::vec3(1.0f)));
        glBindVertexArray(LightVAO);
        for(int i = 0;i<4;i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, Light_Pos[i]);
            model = glm::scale(model,glm::vec3(0.5f));
            glUniformMatrix4fv(glGetUniformLocation(light_ShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);


        //光源位置随时间变化。
        //float posx = 8 * std::sin(glfwGetTime());
        //float posz = 8 * std::cos(glfwGetTime());

        //颜色随时间变化
        //float r = (1 + std::sin(glfwGetTime() * 0.5f))/2;
        //float g = (1 + std::sin(glfwGetTime() * 0.8f))/2;
        //float b = (1 + std::sin(glfwGetTime() * 0.2f))/2;
        //glUniform3fv(glGetUniformLocation(shaderProgram,"LightColor"),1,glm::value_ptr(glm::vec3(r,g,b)));
        //glUniform3fv(glGetUniformLocation(shaderProgram,"LightPos"),1,glm::value_ptr(glm::vec3(LightingModel * LightingPos)));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"view"),1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"projection"),1,GL_FALSE,glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(shaderProgram,"viewPos"),1,glm::value_ptr(CamPos));  //更新观察者位置（相机位置）
        glUniform3fv(glGetUniformLocation(shaderProgram,"spotDir"),1,glm::value_ptr(CamFront));

        //传递所有的点光源位置给fragmentShader中的LightPos数组。
        glUniform3fv(glGetUniformLocation(shaderProgram,"LightPos[0]"),1,glm::value_ptr(Light_Pos[0]));
        glUniform3fv(glGetUniformLocation(shaderProgram,"LightPos[1]"),1,glm::value_ptr(Light_Pos[1]));
        glUniform3fv(glGetUniformLocation(shaderProgram,"LightPos[2]"),1,glm::value_ptr(Light_Pos[2]));
        glUniform3fv(glGetUniformLocation(shaderProgram,"LightPos[3]"),1,glm::value_ptr(Light_Pos[3]));

        //绘制所有盒子
        for(unsigned int i = 0;i<10;i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model,cubePositions[i]);
            float angle = 15.0f * i;
            model = glm::rotate(model,glm::radians(angle),glm::vec3(0.5f,0.3f,0.7f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES,0,36);
        }
        glBindVertexArray(0);

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



