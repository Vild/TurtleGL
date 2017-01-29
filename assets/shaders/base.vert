#version 330

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec3 vertColor;
layout(location = 3) in vec2 vertUV;

out vec3 vPos;
out vec3 vNormal;
out vec3 vColor;
out vec2 vUV;

uniform mat4 m;
uniform mat4 mvp;

void main() {
	vPos = (m * vec4(vertPos, 1.0f)).xyz;
	vNormal = (m * vec4(vertNormal, 1.0f)).xyz;
	/*mat3 normalMatrix = transpose(inverse(mat3(m)));
	vNormal = normalize(normalMatrix * vertNormal);*/
	vColor = vertColor;
	vUV = vertUV;
	gl_Position = mvp * vec4(vertPos, 1.0);
}
