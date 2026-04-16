#version 330 core
out vec4 FragColor;

in vec2 v_TexCoord;

// texture samplers
uniform sampler2D uDiffuseTexture;

void main() {
	FragColor = texture(uDiffuseTexture, v_TexCoord);
}