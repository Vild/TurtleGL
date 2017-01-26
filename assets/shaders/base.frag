#version 330

in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 uv;

out vec4 outColor;

uniform sampler2D tex;
uniform mat4 m;
uniform mat4 mvp;
uniform vec3 diffusePos;

// Eventually we'll replace the shininess variable with something like
// "material.shininess" for each object being rendered.
vec3 phongFunc(){
	vec3 n = normalize((m * vec4(normal, 1)).xyz);
	vec3 pos = (m * vec4(position, 1)).xyz;
	vec3 toLight = normalize(diffusePos - pos);
	vec3 v = normalize(-pos);
	vec3 r = normalize(reflect(-toLight, n));

	vec3 ks = vec3(1.0f, 1.0f, 1.0f);
	float shininess = 2.0f;
	vec3 specularLight = ks * pow(max(0.0, dot(r, v)), shininess);

	vec3 ambientLight = vec3(0.2f, 0.2f, 0.2f);

	float diffuse = max(0.0, dot(n, toLight));
	vec3 diffuseLight = diffuse * color;

	return diffuseLight + ambientLight + specularLight;
}

void main() {
	outColor = texture(tex, uv) * vec4(phongFunc(), 1.0);
}
