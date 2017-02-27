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
uniform mat4 lightSpaceMatrix;

uniform sampler2D defPos;
uniform sampler2D defNormal;
uniform sampler2D defDiffuseSpecular;
uniform sampler2D shadowMap;

uniform bool setting_enableAmbient;
uniform bool setting_enableShadow;
uniform bool setting_enableDiffuse;
uniform bool setting_enableSpecular;
uniform float setting_shininess;

float ShadowCalc(vec4 fragPosLightSpace, vec3 normal, vec3 toLight){
	// Division by w is needed for perspective so it's here for the future, it remains untouched by orthographic projection
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Multiplication and addition by 0.5f to get coord into NDC, [-1,1] -> [0,1]
	projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	// Bias to remove shadow acne.
	float bias = max(0.05f * (1.0 - dot(normal, toLight)), 0.005f);

	float shadow = (currentDepth) > closestDepth ? 1.0 : 0.0;

	return shadow;
}

void main() {
	vec3 pos = texture(defPos, vUV).xyz;
	vec3 normal = texture(defNormal, vUV).xyz;
	vec3 diffuse = texture(defDiffuseSpecular, vUV).xyz;
	float specular = texture(defDiffuseSpecular, vUV).w;
	vec4 fragPosLightSpace = lightSpaceMatrix * vec4(pos, 1.0f);

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
		float shadow = ShadowCalc(fragPosLightSpace, normal, toLight);

		vec3 result = vec3(0);
		if (setting_enableDiffuse)
			result += diffuseLight;
		if (setting_enableSpecular)
			result += specularLight;
		if (setting_enableShadow)
			result *= 1.0f - shadow; // *= because: (1.0 - shadow) * (diffuse + specular)
		if (setting_enableAmbient)
			result += ambientLight;

		lighting = result * diffuse;
	}

	outColor = vec4(lighting, 1);
}
