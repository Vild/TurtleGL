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
	float radius;
	vec3 color;
	float linear;
	vec3 _pad0;
	float quadratic;
};

#define LIGHT_COUNT 16
layout (std140) uniform Lights {
	Light lights[LIGHT_COUNT];
};


void main() {
	vPos = (m * vec4(vertPos, 1.0f)).xyz;
	mat3 normalMatrix = transpose(inverse(mat3(m)));
	vNormal = normalize(normalMatrix * vertNormal);
	vColor = lights[gl_InstanceID].color;
	vUV = vertUV;
	gl_Position = vp * (m * vec4(vertPos, 1.0f));
}
