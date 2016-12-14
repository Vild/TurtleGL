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

void main() {
	vec3 n = normalize((m * vec4(normal, 1)).xyz);
	vec3 pos = (m * vec4(position, 1)).xyz;
	
	vec3 fromLight = normalize(diffusePos - pos);
	float diffuse = max(0, dot(n, fromLight));
	
	outColor = texture(tex, uv) * vec4(color, 1.0) * diffuse;
}
