#version 460 core
#define LIGHT_POS 4
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;  //片段位置
uniform sampler2D Texture1;  //材质纹理
uniform sampler2D Texture2;  //高光纹理
uniform float ka;  //环境光系数
uniform vec3 LightPos[LIGHT_POS];   //点光源位置
uniform vec3 viewPos;  //观察位置 或聚光灯位置
uniform vec3 LightColor;  //点光源颜色
uniform vec3 SpotColor;
uniform float cutOff;  //聚光灯内圆锥范围  单位弧度制
uniform vec3 spotDir;  //聚光灯朝向。即摄像机正面朝向。
uniform float specularParam;  //光泽度
uniform float outerCircle;  //聚光灯外圆锥角度。 弧度制

//返回点光源着色结果
vec3 calcPointLight(vec3 LightPos){
    vec3 common_ambient = ka * LightColor;  //通用的环境光计算
    vec3 kd = vec3(texture(Texture1,TexCoord));
    vec3 norm = normalize(Normal);
    vec3 view_vec = normalize(viewPos-FragPos);  //相机观察向量。注意这里标准化后的
    vec3 dot_light_vec = vec3(LightPos-FragPos);
    float dot_ray_r = length(dot_light_vec);
    float dot_F = 1.0 / (1.0 + dot_ray_r * 0.22 + pow(dot_ray_r,2) * 0.2);
    dot_light_vec = normalize(dot_light_vec);    
    float dot_diff = max(0.0,dot(norm,dot_light_vec));
    vec3 dot_diffuse = dot_diff * LightColor;
    float dot_spec = pow(max(0.0,dot(view_vec,reflect(-dot_light_vec,norm))),specularParam);
    vec3 dot_specular = dot_spec * vec3(texture(Texture2,TexCoord)) * LightColor; 
    vec3 Dot_Light_Color = (common_ambient + dot_diffuse * dot_F)* kd + dot_specular * dot_F;
    return Dot_Light_Color;
}

//返回聚光灯着色结果
vec3 calcSpotLight(){
    vec3 kd = vec3(texture(Texture1,TexCoord));
    vec3 norm = normalize(Normal);
    vec3 spot_light_vec = vec3(viewPos-FragPos);  //聚光灯光线向量.
    vec3 view_vec = normalize(viewPos-FragPos);  //相机观察向量。注意这里标准化后的
    float spot_ray_r = length(spot_light_vec);
    float spot_F = 1.0 / (1.0 + spot_ray_r * 0.045 + pow(spot_ray_r,2) * 0.0075);
    spot_light_vec = normalize(spot_light_vec);
    float spot_diff =  max(0.0,dot(norm,spot_light_vec));
    vec3 spot_diffuse = spot_diff * SpotColor;
    float spot_spec = pow(max(0.0,dot(view_vec,reflect(-spot_light_vec,norm))),specularParam);
    vec3 spot_specular = spot_spec * vec3(texture(Texture2,TexCoord)) * SpotColor;
    vec3 Spot_Light_Color = spot_specular * spot_F + spot_diffuse* kd * spot_F ; 
    return Spot_Light_Color;
}


void main(){
    float theta = dot(normalize(FragPos-viewPos),normalize(spotDir));
    //光线与聚光灯方向夹角。由此判断片段是否在聚光灯可照范围内。
    //在内圆锥内，最终颜色为 聚光灯光线造成的高光、漫反射 + 环境光。
    //如果在外圆锥到内圆锥区间内，插值计算得 inCircle，最终颜色为 聚光灯光线 * inCircle + 点光源的光线
    //外圆锥之外的范围，最终颜色为 点光源光线计算后的颜色。     
    float inCircle = (theta-outerCircle)/(cutOff-outerCircle);
    vec3 result;
    vec3 SpotLight = calcSpotLight();
    //循环遍历点光源数组，计算对于这个片段的最终颜色。根据theta角度判断是否要计算聚光灯颜色。
    for(int i = 0;i<LIGHT_POS;i++)
    {
        result += calcPointLight(LightPos[i]);
    }

    if(theta>=cutOff)
    {
        FragColor = vec4(result + SpotLight,1.0); //加上聚光灯。
    }
    else if(theta >= outerCircle)
    {
        //在内圆锥和外圆锥中间。 聚光灯部分光线 + 点光源
        FragColor = vec4(SpotLight * inCircle + result,1.0);
    }
    else
    {
        FragColor = vec4(result,1.0);
    }

}