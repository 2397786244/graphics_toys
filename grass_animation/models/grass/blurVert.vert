#version 460 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aTexCoords;
out vec2 texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    texcoords = aTexCoords;
    gl_Position = vec4(aPos.x,aPos.y,0,1);
}