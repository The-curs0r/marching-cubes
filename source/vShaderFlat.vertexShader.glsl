#version 450 core

in vec3 vertex;
in vec3 normal;

uniform vec3 lightPos = vec3(0.0f, 0.0f, 0.0f);
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

const vec2 madd = vec2(0.5, 0.5);
out VS_OUT{
	vec2 vUv;
	vec3 worldPos;
	vec3 normal;
	vec3 lightPos;
}vs_out;

void main(void) {
	vs_out.vUv = vertex.xy * madd + madd;
	mat4 normalMatrix = -1.0*transpose(inverse(mv_matrix));
	vs_out.normal = (normalMatrix * vec4(normal, 0.)).xyz;
	//vs_out.normal = (normalMatrix * vec4(1.0,1.0,1.0, 0.)).xyz;
	//vs_out.normal = normal;
	vs_out.lightPos = (proj_matrix * mv_matrix * vec4(lightPos, 1.0f)).xyz;
	gl_Position = proj_matrix * mv_matrix * vec4(vertex, 1.0f);
	vs_out.worldPos = gl_Position.xyz;
}