#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec3 vertColor;
layout (location = 3) in vec2 vertUV;
layout (location = 5) in mat4 m;

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
	float quadratic;
	float yaw;
	float pitch;
	float _pad0;
};

#define LIGHT_COUNT 1
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
