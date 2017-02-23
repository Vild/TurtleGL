#version 330 core

// NOTE: The locations must match ShaderAttributeID
layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec3 vertColor;
layout (location = 3) in vec2 vertUV;
layout (location = 4) in vec3 vertTangent;
layout (location = 5) in mat4 m;

out vec3 vPos;
out vec3 vNormal;
out vec3 vColor;
out vec2 vUV;
out vec4 fragPosLightSpace;
out mat3 vTBN;

uniform mat4 vp;
uniform mat4 lightSpaceMatrix;

mat3 calcTBN(mat3 normalMatrix, vec3 normal) {
	vec3 T = normalize(normalMatrix * vertTangent);
	vec3 N = normalize(normalMatrix * normal);
	// Gram-Schmidt process
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(T, N);
	return mat3(T, B, N);
}

void main() {
	vec4 pos = m * vec4(vertPos, 1.0f);
	vPos = pos.xyz;

	mat3 normalMatrix = transpose(inverse(mat3(m)));
	vNormal = normalize(normalMatrix * vertNormal);

	vColor = vertColor;
	vUV = vertUV;
	fragPosLightSpace = lightSpaceMatrix * pos;
	vTBN = calcTBN(normalMatrix, vertNormal);

	gl_Position = vp * pos;
}
