#version 430 core

layout (location = 0) in vec3 vertPos;
layout (location = 5) in mat4 m;

uniform mat4 v;
uniform mat4 p;

void main(){
	gl_Position = p * v * m * vec4(vertPos, 1.0f);
}
