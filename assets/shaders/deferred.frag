#version 330 core

out vec4 outColor;

in vec3 vColor;
in vec2 vUV;

struct Light {
	vec3 pos;
	float radius;
	vec3 color;
	float linear;
	vec3 _pad0;
	float quadratic;
};

#define LIGHT_COUNT 1
layout (std140) uniform Lights {
	Light lights[LIGHT_COUNT];
};

uniform vec3 cameraPos;

uniform sampler2D defPos;
uniform sampler2D defNormal;
uniform sampler2D defDiffuseSpecular;

void main() {
	vec3 pos = texture(defPos, vUV).xyz;
	vec3 normal = texture(defNormal, vUV).xyz;
	vec3 diffuse = texture(defDiffuseSpecular, vUV).xyz;
	float specular = texture(defDiffuseSpecular, vUV).w;
	vec3 lighting = vec3(0);

	vec3 toCamera = normalize(cameraPos - pos);
	const float shininess = 64.0f;
	for (int i = 0; i < LIGHT_COUNT; i++) {
		// Diffuse
		vec3 toLight = normalize(lights[i].pos - pos);
		vec3 diffuseLight = max(dot(normal, toLight), 0.0f) * lights[i].color;

		// Specular (Blinn-phong)
		vec3 halfwayDir = normalize(toLight + toCamera);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
		vec3 specularLight = spec * specular * lights[i].color;


		// Ambient
		vec3 ambientLight = lights[i].color * 0.1f;

		lighting = (ambientLight + diffuseLight + specularLight) * diffuse;
	}

	outColor = vec4(lighting, 1);
}
