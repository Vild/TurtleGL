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

float ShadowCalc(vec4 fragPosLightSpace, vec3 normal, vec3 toLight){
	// Division by w is needed for perspective so it's here for the future, it remains untouched by orthographic projection
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Multiplication and addition by 0.5f to get coord into NDC, [-1,1] -> [0,1]
	projCoords = (projCoords * vec3(0.5)) + vec3(0.5);

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
		vec3 ambientLight = diffuse * 0.1f;

		// Shadow
		float shadow = ShadowCalc(fragPosLightSpace, normal, toLight);

		//lighting = (ambientLight + diffuseLight + specularLight) * diffuse;

		lighting = (ambientLight + (1.0 - shadow) * (diffuseLight + specularLight)) * diffuse;
	}

	outColor = vec4(lighting, 1);
}
