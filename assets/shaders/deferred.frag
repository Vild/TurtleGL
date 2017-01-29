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
/*
	vec3 diffusePos = lightsPos[0];

	vec3 toLight = normalize(diffusePos - pos);
	vec3 v = normalize(-pos);
	vec3 r = normalize(reflect(-toLight, normal));

	vec3 ks = vec3(1.0f, 1.0f, 1.0f);
	float shininess = specular;
	vec3 specularLight = ks * pow(max(0.0, dot(r, v)), shininess);

	vec3 ambientLight = vec3(0.2f, 0.2f, 0.2f) * diffuse;

	vec3 diffuseLight = max(0.0, dot(normal, toLight)) * diffuse;

	outColor = vec4(diffuseLight + ambientLight + specularLight, 1);*/

	vec3 lighting = vec3(0,0,0);//diffuse * 0.1;

	for (int i = 0; i < LIGHT_COUNT; i++) {
		vec3 toLight = normalize(lightsPos[i] - pos);
		vec3 diffuseColor = max(dot(normal, toLight), 0.0) * diffuse;// * lightsColor[i];
		lighting += diffuseColor;
	}

	outColor = vec4(diffuse, 1);
}
