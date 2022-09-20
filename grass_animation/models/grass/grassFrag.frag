#version 460 core
in vec2 TexCoords;
uniform sampler2D grass;
out vec4 FragColor;

in float fragDepth;
uniform float far_plane;
uniform float near_plane;

float calcBlur(float z){
    //深度小于near_plane、深度大于far_plane、在两点之间三种情况
    if(z >= near_plane && z <= far_plane){
        return 0.f ;//完全清晰。
    }
    else if(z < near_plane){
        return clamp(1,0,(near_plane - z) / 10.0);
    }
    else if(z > far_plane){
        return clamp(1,0,(z-far_plane) / 10.0);
    }
}

void main() {
    vec4 sc = texture(grass,TexCoords);
    if(sc.a < 0.1)
    {
        discard;
    }
    FragColor = sc;
    FragColor.a = calcBlur(fragDepth);  //计算模糊因子。下一次pass的时候对图像进行模糊操作。
}