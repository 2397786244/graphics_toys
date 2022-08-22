#include <locale>
#include "Shader.h"
#include "fstream"
#include "iostream"
namespace toy{
    //着色器初始化
    Shader::Shader(const char *vertexShaderPath, const char *fragShaderPath,const char * geoShaderPath)
    {
        //从两个path路径读取着色器程序文件。
        const char * vert,*frag;
        unsigned int vertShader,fragShader;
        std::string verttemp,fragtemp;
        std::ifstream vo,fo;
        vo.open(vertexShaderPath,std::ios::in);
        fo.open(fragShaderPath,std::ios::in);
        std::string line;
        while(!vo.eof()){
            std::getline(vo,line);
            verttemp += line + '\n';
        }
        vert = verttemp.c_str();
        while(!fo.eof())
        {
            std::getline(fo,line);
            fragtemp += line + '\n';
        }
        frag = fragtemp.c_str();
        vo.close();fo.close();
        int success;
        vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader,1,&vert, nullptr);
        glCompileShader(vertShader);
        glGetShaderiv(vertShader,GL_COMPILE_STATUS,&success);
        if(!success){
            std::cerr << "VertexShader Compile error!\n";
            exit(-1);
        }
        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader,1,&frag, nullptr);
        glCompileShader(fragShader);
        glGetShaderiv(fragShader,GL_COMPILE_STATUS,&success);
        if(!success){
            std::cerr << "FragmentShader Compile error!\n";
            exit(-1);
        }

        _shaderProgram = glCreateProgram();

        glAttachShader(_shaderProgram,vertShader);
        glAttachShader(_shaderProgram,fragShader);
        glLinkProgram(_shaderProgram);
        //链接是否成功的检查
        glad_glGetProgramiv(_shaderProgram,GL_LINK_STATUS,&success);
        if(!success){
            std::cerr << "Link Failed!\n";
            exit(-1);
        }
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
    }

    void Shader::use() const{
        glUseProgram(_shaderProgram);
    }

    void Shader::send1f(const char * uname,float _1f) {
        assert(uname);
        glUniform1f(glGetUniformLocation(_shaderProgram,uname),_1f);
    }
    void Shader::send1i(const char *uname, int _1i) {
        assert(uname);
        glUniform1i(glGetUniformLocation(_shaderProgram,uname),_1i);
    }

    void Shader::send2fv(const char *uname, glm::vec2 _2fv) {
        assert(uname);
        glUniform2fv(glGetUniformLocation(_shaderProgram,uname),1,glm::value_ptr(_2fv));
    }
    void Shader::send3fv(const char *uname, glm::vec3 _3fv) {
        assert(uname);
        glUniform3fv(glGetUniformLocation(_shaderProgram,uname),1,glm::value_ptr(_3fv));
    }
    void Shader::sendmat4fv(const char *uname, glm::mat4 _mat4fv) {
        assert(uname);
        glUniformMatrix4fv(glGetUniformLocation(_shaderProgram,uname),1,GL_FALSE,glm::value_ptr(_mat4fv));
    }
    unsigned int Shader::getShaderProgramID() {return _shaderProgram;}
}