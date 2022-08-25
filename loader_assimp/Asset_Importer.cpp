#include "Asset_Importer.h"
#include <utility>

imp_Mesh::imp_Mesh(const std::vector<imp_Vertex> &iv, const std::vector<imp_Texture> &it,
                   const std::vector<unsigned int> &ind,bool HasMaterial_Diff,bool HasMaterial_Spec,glm::vec3 ambient,glm::vec3 diff,glm::vec3 spec) {
    this->_v = iv;
    this->_t = it;
    this->_ind = ind;
    this->_ambient = ambient;
    this->_diff = diff;
    this->_spec = spec;
    this->_HasMaterial_Diff = HasMaterial_Diff;
    this->_HasMaterial_Spec  =HasMaterial_Spec;
    setupAsset();
}

void imp_Mesh::Draw(toy::Shader shader) {
    shader.use();
    //传递纹理到FragmentShader。 支持漫反射贴图(TexDiff_N)、高光贴图(TexSpec_N)
    std::string name;
    unsigned int diff_i=1,spec_i=1;
    shader.use();
    shader.send3fv("Ka", this->_ambient);
    if(!_HasMaterial_Diff) {
        shader.send3fv("Kd", this->_diff);
    }
    if(!_HasMaterial_Spec){
        shader.send3fv("Ks",this->_spec);
    }

    for(int i =0;i<_t.size();i++){
        glActiveTexture(GL_TEXTURE0+i);  //激活纹理
        name = _t[i].type;
        if(name == "TexDiff"){
            name += '_' + std::to_string(diff_i++);
        }
        else if(name == "TexSpec"){
            name += '_' + std::to_string(spec_i++);
        }
        shader.use();
        shader.send1i(name.c_str(),i);
        glBindTexture(GL_TEXTURE_2D,_t[i].TexInd); //跟纹理ID绑定
    }
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,_ind.size(),GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
}

void imp_Mesh::setupAsset() {
    //根据vertex数组 配置 VAO、VBO，根据 索引数组配置 EBO
    glGenVertexArrays(1,&this->VAO);
    glGenBuffers(1,&this->VBO);
    glGenBuffers(1,&this->EBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER,this->VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->EBO);

    //顶点数据设置
    glBufferData(GL_ARRAY_BUFFER,sizeof(imp_Vertex) * this->_v.size(),&this->_v[0],GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int) * this->_ind.size(),&this->_ind[0],GL_STATIC_DRAW);
    //属性指针配置
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(imp_Vertex),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(imp_Vertex),(void*) offsetof(imp_Vertex,vNormal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(imp_Vertex),(void*) offsetof(imp_Vertex,vTexCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}


namespace toy{
    Model::Model(std::string model_path)
{
    loadModel(std::move(model_path));
}

    void Model::Draw(const toy::Shader &shader) {
        for(auto & each:meshes){
            each.Draw(shader);
        }
}

    void Model::loadModel(std::string path) {
        Assimp::Importer importer;
        const aiScene * scene = importer.ReadFile(path,aiProcess_Triangulate|aiProcess_FlipUVs);
        if(!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        {
            std::cerr << "Read Model File Failed!" << std::endl;
            exit(-1);
        }
        if(!scene->HasMaterials()){
            std::cout << "Mention:This Model Has No Material\n" << std::endl;
        }
        this->dictionary = path.substr(0,path.find_last_of('/'));
        travelNodeToMeshes(scene->mRootNode,scene);

}

    void Model::travelNodeToMeshes(aiNode *node, const aiScene *scene) {
        //获取该结点下的网格。添加到meshes数组中。
        for(int i = 0;i<node->mNumMeshes;i++){
            aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(Model::processMeshes(mesh,scene));
        }
        //递归处理子节点的meshes
        for(int i = 0;i<node->mNumChildren;i++){
            travelNodeToMeshes(node->mChildren[i],scene);
        }

    }

    imp_Mesh Model::processMeshes(aiMesh *mesh, const aiScene *scene)
    {
        //获取mesh的顶点位置、法向量、纹理坐标、绘制索引、对应的纹理图像.
        std::vector<imp_Vertex>vertex;
        std::vector<imp_Texture>tex;
        std::vector<unsigned int> ind;
        bool diff_check = false,spec_check = false;
        glm::vec3 diff,spec,ambient;
        int i,j;
        glm::vec3 vp,vn;glm::vec2 vt;
        for(i = 0;i<mesh->mNumVertices;i++){
            vp = glm::vec3(mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z);
            if(mesh->HasNormals())
                vn = glm::vec3(mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z);
            else
                vn = {0.0f,0.0f,0.0f};
            if(mesh->HasTextureCoords(0)) //一个顶点最多支持8个纹理坐标。这里使用第一个
                vt = glm::vec2(mesh->mTextureCoords[0][i].x,mesh->mTextureCoords[0][i].y);
            else
                vt = glm::vec2 {0.f,0.f};

            vertex.push_back(imp_Vertex{vp,vn,vt});
        }

        //添加索引
        for(i = 0;i<mesh->mNumFaces;i++){
            aiFace face = mesh->mFaces[i];
            for(j = 0;j<face.mNumIndices;j++){
                ind.push_back(face.mIndices[j]);
            }
        }

        aiColor3D cd,cs,ca;
        //纹理图像的读取。
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        //material 包括多种类型的纹理 下面编写获取漫反射贴图和高光贴图两种
        std::vector<imp_Texture> diffMaps = loadTexture(material,aiTextureType_DIFFUSE,"TexDiff");
        tex.insert(tex.end(),diffMaps.begin(),diffMaps.end());
        std::vector<imp_Texture> specMaps = loadTexture(material,aiTextureType_SPECULAR,"TexSpec");
        tex.insert(tex.end(),specMaps.begin(),specMaps.end());
        //获取环境光、漫反射、高光系数。如果没有相应的贴图，就使用该系数。
        material->Get(AI_MATKEY_COLOR_AMBIENT,ca);
        ambient = {ca.r,ca.g,ca.b};
        material->Get(AI_MATKEY_COLOR_DIFFUSE,cd);
        diff = {cd.r,cd.g,cd.b};
        material->Get(AI_MATKEY_COLOR_SPECULAR,cs);
        spec = {cs.r,cs.g,cs.b};
        if(!diffMaps.empty())
        {  //检查是否有漫反射贴图
            diff_check = true;
        }
        if(!specMaps.empty())
        {  //检查是否有高光贴图
            spec_check = true;
        }
        //TODO:支持法线贴图
        return {vertex, tex, ind,diff_check,spec_check,ambient,diff,spec};
    }

    //从material中获取纹理名（还要判断该纹理是否已经被加载过） 然后根据图像名 加上目录名 调用loadTextureFromFile从本地加载该纹理。
    std::vector<imp_Texture> Model::loadTexture(aiMaterial *material, aiTextureType type, std::string typeName)
    {
        aiString fileName;
        std::vector<imp_Texture> texs;
        imp_Texture temp;
        for(int i = 0;i<material->GetTextureCount(type);i++)
        {
            material->GetTexture(type,i,&fileName);
            if(this->loaded_tex.find(std::string(fileName.C_Str())) != loaded_tex.end())
            {
                //可以找到，则直接把该Texture放到这个imp_texture数组中。
                texs.push_back(loaded_tex[std::string(fileName.C_Str())]);
            }
            else{
                //没有找到则从本地读取
                temp.TexInd = loadTextureFromFile(fileName.C_Str(),this->dictionary);
                temp.type = typeName;
                loaded_tex[fileName.C_Str()] = temp;
                texs.push_back(temp);
            }
        }
        return texs;
    }

    unsigned int Model::loadTextureFromFile(std::string picName, std::string dictionary)
    {
        unsigned int TexID;
        std::string fileName = dictionary + '/' + picName;
        glGenTextures(1,&TexID);
        int width,height,nrComponents;
        GLint format;
        unsigned char * data = stbi_load(fileName.c_str(),&width,&height,&nrComponents,0);

        if (data)
        {
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, TexID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << fileName << std::endl;
            stbi_image_free(data);
        }

        return TexID;
    }


}