#version 330 core

out vec4 outColor;

in vec3 vColor;
in vec2 vUV;

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

uniform vec3 cameraPos;

uniform sampler2D defPos;
uniform sampler2D defNormal;
uniform sampler2D defDiffuseSpecular;
uniform sampler2D defShadowCoord;
uniform sampler2DShadow shadowMap;

uniform bool setting_enableAmbient;
uniform bool setting_enableShadow;
uniform bool setting_enableDiffuse;
uniform bool setting_enableSpecular;
uniform float setting_shininess;
uniform int setting_pcfSamples;

float random(vec4 seed) {
	float dot_product = dot(seed, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

void main() {
	vec3 pos = texture(defPos, vUV).xyz;
	vec3 normal = texture(defNormal, vUV).xyz;
	vec3 diffuse = texture(defDiffuseSpecular, vUV).xyz;
	float specular = texture(defDiffuseSpecular, vUV).w;
	vec4 shadowCoord = texture(defShadowCoord, vUV);

	vec3 lighting = vec3(0);

	vec3 toCamera = normalize(cameraPos - pos);
	for (int i = 0; i < LIGHT_COUNT; i++) {
		// Diffuse
		vec3 toLight = normalize(lights[i].pos - pos);
		vec3 diffuseLight = max(dot(normal, toLight), 0.0f) * lights[i].color;

		// Specular (Blinn-phong)
		vec3 halfwayDir = normalize(toLight + toCamera);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), setting_shininess);
		vec3 specularLight = spec * specular * lights[i].color;

		// Ambient
		vec3 ambientLight = diffuse * 0.1f;

		// Shadow
		float shadow = 1;

		if (setting_enableShadow && shadowCoord.w > 1) {
			shadow = 0;
			int half = setting_pcfSamples/2;
			for (int x = 0; x < setting_pcfSamples; x++) {
				for(int y = 0; y < setting_pcfSamples; y++){
					shadow += textureProjOffset(shadowMap, shadowCoord, ivec2(x-half, y-half));
				}
			}
			shadow /= setting_pcfSamples * setting_pcfSamples;
		}

		vec3 result = vec3(0);
		if (setting_enableDiffuse)
			result += diffuseLight;
		if (setting_enableSpecular)
			result += specularLight;

		result *= shadow;

		if (setting_enableAmbient)
			result += ambientLight;

		lighting = result * diffuse;
	}

	outColor = vec4(lighting, 1);
}
