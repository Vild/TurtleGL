#version 330 core

// NOTE: The locations must match ShaderAttributeID
layout (location = 0) in vec3 vertPos;
out vec3 worldPos;

uniform mat4 vp;

void main() {
	vec4 pos = m * vec4(vertPos, 1.0f);

	vColor = vertColor;
	vUV = vertUV;

	gl_Position = vp * pos;
}
