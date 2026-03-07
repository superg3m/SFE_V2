#version 330 core

in vec2 v_TexCoord;
out vec4 FragColor;

in vec3 v_Color;

void main() {
    FragColor = vec4(v_Color, 1);
}