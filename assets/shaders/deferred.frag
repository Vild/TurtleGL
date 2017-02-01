#version 330 core

out vec4 outColor;

in vec3 vColor;
in vec2 vUV;

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

uniform vec3 cameraPos;

uniform sampler2D defPos;
uniform sampler2D defNormal;
uniform sampler2D defDiffuseSpecular;


void main() {
	vec3 pos = texture(defPos, vUV).rgb;
	vec3 normal = texture(defNormal, vUV).rgb;
	vec3 diffuse = texture(defDiffuseSpecular, vUV).rgb * vColor;
	float specular = texture(defDiffuseSpecular, vUV).a;

	vec3 lighting = diffuse * 0.05;

	vec3 toCamera = normalize(cameraPos - pos);

	for (int i = 0; i < LIGHT_COUNT; i++) {
		//lighting += (1.0/LIGHT_COUNT);
		vec3 toLight = normalize(lights[i].pos- pos);
		vec3 lightColor = max(dot(normal, toLight), 0.0) * diffuse * lights[i].color;
		lighting += lightColor*(1.0/LIGHT_COUNT);
	}


	outColor = vec4(lighting, 1);
}
