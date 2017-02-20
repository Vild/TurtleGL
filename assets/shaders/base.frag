#version 330 core

// Deferred output
layout (location = 0) out vec3 defPos;
layout (location = 1) out vec3 defNormal;
layout (location = 2) out vec4 defDiffuseSpecular;

in vec3 vPos;
in vec3 vNormal;
in vec3 vColor;
in vec2 vUV;

uniform sampler2D diffuseTexture;

void main() {
	defPos = vPos;
	defNormal = vNormal;
	defDiffuseSpecular.rgb = vColor * texture(diffuseTexture, vUV).rgb;
	defDiffuseSpecular.a = 0.5f;
}
