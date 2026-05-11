#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aColor; // one day for vertex painting

out vec2 v_TexCoord;
out vec3 v_WorldPosition;
out vec3 v_Normal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
	mat4 model = uModel;

	v_TexCoord = aTexCoord;
    v_WorldPosition = vec3(model * vec4(aPos, 1.0));
	v_Normal = mat3(transpose(inverse(uModel))) * aNormal; 

	gl_Position = uProjection * uView * vec4(v_WorldPosition, 1.0);
}