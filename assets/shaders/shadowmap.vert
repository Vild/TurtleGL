#version 330 core

in vec3 vertPos;
in mat4 m;

uniform mat4 lightSpaceMatrix;

void main(){
	gl_Position = lightSpaceMatrix * m * vec4(vertPos, 1.0f);
}