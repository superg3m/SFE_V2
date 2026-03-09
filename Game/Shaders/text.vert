#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 v_Color;

uniform float offset_x;
uniform float offset_y;

void main() {
    v_Color = aColor;
    gl_Position = vec4(aPos.x + offset_x, aPos.y + offset_y, aPos.z, 1);
}