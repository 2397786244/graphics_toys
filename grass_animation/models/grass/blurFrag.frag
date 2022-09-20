#version 460 core
in vec2 texcoords;
uniform sampler2D texture1;
out vec4 FragColor;

const float offset = 1.0/100.0;
void main() {
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    vec4 originColor = texture(texture1,texcoords);
    vec3 blurColor = vec3(0.0,0.0,0.0);
    vec2 offset_pos = vec2(0.0);
    vec4 curColor = vec4(0.0);
    for(int i = 0;i<9;i++){
        offset_pos = offsets[i] + texcoords;
        curColor = texture(texture1,offset_pos);
        blurColor += originColor.rgb  * (1.0 - originColor.a) + curColor.rgb * originColor.a;
    }
    FragColor = vec4(blurColor.rgb/9.0,1.0);
}