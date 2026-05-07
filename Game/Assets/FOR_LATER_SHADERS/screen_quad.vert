#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 3) in vec3 aColor;
layout (location = 4) in vec2 aOffset;

out vec3 v_Color;

void main() {
    gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
    v_Color = aColor;
}