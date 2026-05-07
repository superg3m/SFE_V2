#version 330 core

in vec3 v_TexCoord;
uniform samplerCube uSkyboxDay;
uniform samplerCube uSkyboxNight;
uniform float uBlend;

out vec4 FragColor;

void main() {
	vec3 sample = mix(texture(uSkyboxDay, v_TexCoord).rgb, texture(uSkyboxNight, v_TexCoord).rgb, uBlend);
    FragColor = vec4(sample, 1.0);
}