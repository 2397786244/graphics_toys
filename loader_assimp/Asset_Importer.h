#ifndef OPENGL_TEST_ASSET_IMPORTER_H
#define OPENGL_TEST_ASSET_IMPORTER_H
#include "glm/glm.hpp"
#include "Shader.h"
#include "vector"
#include "string"
#include <assimp/Importer.hpp>
#include  <assimp/scene.h>
#include <assimp/postprocess.h>
#include "map"
#include "iostream"
#include "stb_image.h"

struct imp_Vertex{
    glm::vec3 vPos;
    glm::vec3 vNormal;
    glm::vec2 vTexCoords;
};
struct imp_Texture{
    unsigned int TexInd;  //纹理索引
    std::string type;
};
class imp_Mesh{
public:
    //资产导入类使用顶点向量、纹理向量、绘制索引向量初始化。
    imp_Mesh(const std::vector<imp_Vertex> & iv,const std::vector<imp_Texture> & it,const std::vector<unsigned int> & ind,bool HasMaterial_Diff,bool HasMaterial_Spec,glm::vec3 ambient = {0.f,0.f,0.f},glm::vec3 diff = {0.f,0.f,0.f},glm::vec3 spec = {0.f,0.f,0.f});
    void setupAsset();
    void Draw(toy::Shader shader);
private:
    unsigned int VAO,VBO,EBO;
    bool _HasMaterial_Diff;
    bool _HasMaterial_Spec;
    std::vector<imp_Vertex> _v;
    std::vector<imp_Texture>_t;
    std::vector<unsigned int> _ind;
    glm::vec3 _ambient,_diff,_spec;
};

namespace toy {
    class Model{
    public:
        Model(std::string model_path);
        void Draw(const Shader & shader);
    private:
        void loadModel(std::string path);
        void travelNodeToMeshes(aiNode * node,const aiScene * scene);
        imp_Mesh processMeshes(aiMesh * mesh,const aiScene *scene);
        std::vector<imp_Texture> loadTexture(aiMaterial * material,aiTextureType type,std::string typeName);
        unsigned int loadTextureFromFile(std::string picName,std::string dictionary);
        std::map<std::string,imp_Texture> loaded_tex;  //记录已经加载的纹理。
        std::string dictionary;  //模型文件目录
        std::vector<imp_Mesh> meshes;
    };
}
#endif //OPENGL_TEST_ASSET_IMPORTER_H
