#version 330

out vec4 outColor;

//in vec3 vPos;
//in vec3 vNormal;
//in vec3 vColor;
//in vec2 vUV;

uniform vec3 lightColor;

void main() {
	outColor = vec4(lightColor, 1);
}
