#version 430 core

// Deferred output
layout (location = 0) out vec3 defPos;
layout (location = 1) out vec3 defNormal;
layout (location = 2) out vec4 defDiffuseSpecular;

in vec3 vPos;
in vec3 vNormal;
in vec3 vColor;
in vec2 vUV;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform float setting_defaultSpecular;

void main() {
	defPos = vPos;

	//vec3 normal = normalize(texture2D(normalTexture, vUV).rgb * 2.0 - vec3(1.0));
	//defNormal = normalize(gTBN * normal);
	defNormal = normalize(vNormal);

	defDiffuseSpecular.rgb = vColor * texture(diffuseTexture, vUV).rgb;
	defDiffuseSpecular.a = setting_defaultSpecular;
}
