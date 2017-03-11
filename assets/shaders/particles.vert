#version 330 core

// NOTE: The locations must match ShaderAttributeID
layout (location = 0) in vec3 vertPos;
//layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec3 vertColor;
layout (location = 3) in vec2 vertUV;
layout (location = 5) in mat4 m;

out vec3 vPos;
//out vec3 vNormal;
out vec3 vColor;
out vec2 vUV;

uniform vec3 particleCenter;
uniform vec3 cameraRight_wPos;
uniform vec3 cameraUp_wPos;
uniform vec3 billboardSize;
uniform mat4 worldView;

void main() {
	vPos = particleCenter + (vec3(cameraRight_wPos * vertPos.x * billboardSize.x) + vec3(cameraUp_wPos * vertPos.y * billboardSize.y));
	//vNormal = vertNormal;
	vColor = vertColor;
	vUV = vertUV;

	gl_Position = worldView * m * vec4(vPos, 1.0f);
}
