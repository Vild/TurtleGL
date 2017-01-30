#version 330

out vec4 outColor;

in vec3 vColor;
in vec2 vUV;

const int LIGHT_COUNT = 16;
uniform vec3 lightsPos[LIGHT_COUNT];
uniform vec3 lightsColor[LIGHT_COUNT];
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
		vec3 toLight = normalize(lightsPos[i] - pos);
		vec3 lightColor = max(dot(normal, toLight), 0.0) * diffuse * lightsColor[i];
		lighting += lightColor*(1.0/LIGHT_COUNT);
	}


	outColor = vec4(lighting, 1);
}
