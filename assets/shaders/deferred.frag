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

#define LIGHT_COUNT 16
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
	vec3 diffuse = texture(defDiffuseSpecular, vUV).xyz * vColor;
	float specular = texture(defDiffuseSpecular, vUV).w;

	vec3 lighting = diffuse * 0.1;

	vec3 toCamera = normalize(cameraPos - pos);

	for (int i = 0; i < LIGHT_COUNT; i++) {
		float distance = length(lights[i].pos - pos);
		if(distance < lights[i].radius) {
			vec3 toLight = normalize(lights[i].pos - pos);
			vec3 lightColor = max(dot(normal, toLight), 0.0) * diffuse * lights[i].color;
			// Specular
			vec3 halfwayDir = normalize(toLight + toCamera);
			float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
			vec3 specular = lights[i].color * spec * specular;
			// Attenuation
			float attenuation = 1.0;// / (1.0 + lights[i].linear * distance + lights[i].quadratic * distance * distance);
			lightColor *= attenuation;
			specular *= attenuation;
			lighting += lightColor + specular;
		}
	}

	outColor = vec4(lighting, 1);
}
