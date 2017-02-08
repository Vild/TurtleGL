#version 330 core

out vec4 outColor;

in vec2 vUV;

uniform sampler2D diffuseTexture;

void main() {
	outColor = texture(diffuseTexture, vUV);
}
