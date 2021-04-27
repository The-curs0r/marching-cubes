#version 450 core

in vec3 vertex;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
const vec2 madd = vec2(0.5, 0.5);
out VS_OUT{
	vec2 vUv;
	vec3 worldPos;
}vs_out;

void main(void) {
	vs_out.vUv = vertex.xy * madd + madd;
	gl_Position = proj_matrix * mv_matrix * vec4(vertex, 1.0f);
	vs_out.worldPos = gl_Position.xyz;
}