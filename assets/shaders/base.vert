#version 330 core

in vec3 vertPos;
in vec3 vertNormal;
in vec3 vertColor;
in vec2 vertUV;
in vec3 vertTangent;
in mat4 m;

out vec3 vPos;
out vec3 vNormal;
out vec3 vColor;
out vec2 vUV;
out mat3 vTBN;

uniform mat4 vp;


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
	vTBN = calcTBN(normalMatrix, vertNormal);

	gl_Position = vp * pos;
}
