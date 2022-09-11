#version 460 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec2 aTexCoords;
layout(location=2) in mat4 model;  //四倍vec4大小。
uniform mat4 view;
uniform mat4 projection;
uniform float time;
out vec2 TexCoords;

void main() {
    vec4 oPos = model * vec4(aPos, 1.0);
    if(aTexCoords.y > 0.8) {
        oPos.x = oPos.x + time;
        oPos.z = oPos.z + time * 0.2;
    }
    TexCoords = aTexCoords;
    gl_Position = projection * view * oPos;
}