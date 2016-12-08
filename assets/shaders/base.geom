#version 330
#extension GL_ARB_geometry_shader4 : enable

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out; //3*2 = triangle*2

//in vec3 vNormal[];
in vec3 vColor[];
in vec2 vUV[];

out vec3 normal;
out vec3 color;
out vec2 uv;

uniform mat4 mvp;

void main() {
	int i, j;
	vec4 offset = vec4(0);

	vec3 e0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 e1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	
	vec3 n = cross(e0, e1);
	
	for (j = 0; j < 2; j++) {
		for (i = 0; i < gl_in.length; i++) {
			gl_Position = mvp * (gl_in[i].gl_Position + offset);
			normal = n;
			color = vColor[i];
			uv = vUV[i];
			EmitVertex();
		}
		EndPrimitive();
		offset = vec4(n, 0);
	}
}
