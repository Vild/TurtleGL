#version 330 core

in vec3 vertPos;
in vec2 vertUV;
in mat4 m;

out vec2 vUV;

uniform mat4 vp;

void main() {
	vUV = vertUV;
	vec4 pos = (vp * m) * vec4(vertPos, 1.0);
	gl_Position = pos.xyww;
}
