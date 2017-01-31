#version 330 core

in vec3 vertPos;
in vec3 vertNormal;
in vec3 vertColor;
in vec2 vertUV;
in mat4 m;
//in vec3 lightColor;

out vec3 vPos;
out vec3 vNormal;
out vec3 vColor;
out vec2 vUV;

uniform mat4 vp;

struct Light {
	vec3 pos;
	float _p0;
	vec3 color;
	float _p1;
};

#define LIGHT_COUNT 16
layout (std140) uniform Lights {
	Light lights[LIGHT_COUNT];
};


void main() {
	vPos = (m * vec4(vertPos, 1.0f)).xyz;
	vNormal = (m * vec4(vertNormal, 1.0f)).xyz;
	vColor = lights[gl_InstanceID].color;
	vUV = vertUV;
	gl_Position = vp * m * vec4(vertPos, 1.0);
}
