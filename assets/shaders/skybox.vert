#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 3) in vec2 vertUV;
layout (location = 5) in mat4 m;

out vec2 vUV;

uniform mat4 vp;

void main() {
	vUV = vertUV;
	vec4 pos = (vp * m) * vec4(vertPos, 1.0);
	gl_Position = pos.xyww;
}
