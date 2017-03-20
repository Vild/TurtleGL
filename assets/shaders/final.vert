#version 430 core
// NOTE: The locations must match ShaderAttributeID
layout (location = 0) in vec3 vertPos;
layout (location = 3) in vec2 vertUV;
layout (location = 5) in mat4 m;

uniform mat4 vp;

out vec2 vUV;

void main() {
	vUV = vertUV;
	gl_Position = vp * m * vec4(vertPos, 1.0);
}
