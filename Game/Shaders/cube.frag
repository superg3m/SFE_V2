#version 330 core
out vec4 FragColor;

in vec2 v_TexCoord;

// texture samplers
uniform sampler2D container;
uniform sampler2D face;

void main() {
	FragColor = mix(texture(container, v_TexCoord), texture(face, v_TexCoord), 0.2);
}