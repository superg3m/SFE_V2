#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 v_Color;
uniform mat4 uModel;

void main() {
    v_Color = aColor;
    gl_Position = uModel * vec4(aPos, 1);
}