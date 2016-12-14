#version 330

in vec4 position;
in vec3 normal;
in vec3 color;
in vec2 uv;

out vec4 outColor;

uniform sampler2D tex;
uniform mat4 mvp;
uniform mat4 mv;
uniform vec3 diffusePos;

void main() {
	vec3 dir = -(mv * vec4(diffusePos, 0)).xyz;
	float diffuse = max(0, dot(normal, dir));
	
	outColor = texture(tex, uv) * vec4(color, 1.0) * diffuse;
}
