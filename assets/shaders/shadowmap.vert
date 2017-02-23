#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 5) in mat4 m;

uniform mat4 lightSpaceMatrix;

void main(){
	gl_Position = lightSpaceMatrix * m * vec4(vertPos, 1.0f);
}
