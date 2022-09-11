#version 460 core
in vec2 TexCoords;
uniform sampler2D grass;
out vec4 FragColor;
void main() {
    vec4 sc = texture(grass,TexCoords);
    if(sc.a < 0.1)
    {
        discard;
    }
    FragColor = sc;
}