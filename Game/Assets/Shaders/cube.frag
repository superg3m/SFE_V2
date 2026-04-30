#version 330 core
out vec4 FragColor;

in vec2 v_TexCoord;

// texture samplers
uniform sampler2D uContainer;
uniform sampler2D uFace;

void main() {
	FragColor = // mix(texture(uContainer, v_TexCoord), texture(uFace, v_TexCoord), 0.2);
}