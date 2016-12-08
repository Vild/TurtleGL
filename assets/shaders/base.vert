#version 330

layout(location = 0) in vec3 vertPos;
//layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec3 vertColor;
layout(location = 3) in vec2 vertUV;

//out vec3 vNormal;
out vec3 vColor;
out vec2 vUV;


void main() {
	//vNormal = vertNormal;
	vColor = vertColor;
	vUV = vertUV;
	gl_Position = vec4(vertPos, 1.0);
}
