#version 460 core
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
out vec4 FragColor;
uniform sampler2D TexDiff_1;  //材质纹理
uniform sampler2D TexSpec_1;  //高光纹理
uniform vec3 Ka;  //环境光系数
uniform vec3 LightPos;   //点光源位置
uniform vec3 viewPos;  //观察位置 或聚光灯位置
uniform vec3 LightColor;  //点光源颜色
uniform float specularParam;  //光泽度
uniform vec3 Kd;
uniform vec3 Ks;
//返回点光源着色结果
vec3 calcPointLight(){
    vec3 common_ambient = Ka * LightColor;  //通用的环境光计算
    vec3 kd = vec3(texture(TexDiff_1,TexCoord));
    vec3 norm = normalize(Normal);
    vec3 view_vec = normalize(viewPos-FragPos);  //相机观察向量。注意这里标准化后的
    vec3 dot_light_vec = vec3(LightPos-FragPos);
    float dot_ray_r = length(dot_light_vec);
    float dot_F = 1.0 / (1.0 + dot_ray_r * 0.22 + pow(dot_ray_r,2) * 0.2);
    dot_light_vec = normalize(dot_light_vec);    
    float dot_diff = max(0.0,dot(norm,dot_light_vec));
    vec3 dot_diffuse = dot_diff * LightColor;
    float dot_spec = pow(max(0.0,dot(view_vec,reflect(-dot_light_vec,norm))),specularParam);
    vec3 ks = vec3(texture(TexSpec_1,TexCoord));
    vec3 dot_specular = dot_spec * (ks+Ks) * LightColor; 
    vec3 Dot_Light_Color = (common_ambient + dot_diffuse * dot_F)* (kd + Kd) + dot_specular * dot_F;
    return Dot_Light_Color;
}

void main(){
    vec3 PointLightColor = calcPointLight();
    FragColor = vec4(PointLightColor,1.0);
}
