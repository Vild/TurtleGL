#version 430 core

in vec2 vUV;
out vec4 fragColor;

uniform sampler2D gaussianImage;

void main() {
	vec3 color = texture(gaussianImage, vUV).rgb;
    fragColor = vec4(color, 1.0);
}