#version 450 core
in vec3 vertex;
in vec3 normal;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
const vec2 madd = vec2(0.5, 0.5);
out VS_OUT{
	vec2 vUv;
    vec3 normal;
    vec3 worldPos;
}vs_out;
void main(void) {
	vs_out.vUv = vertex.xy * madd + madd;
    vs_out.normal = normal;
	gl_Position = proj_matrix * mv_matrix * vec4(vertex, 1.0f);
    vs_out.worldPos = (proj_matrix * mv_matrix * vec4(vertex, 1.0f)).xyz;
    vs_out.worldPos = vertex;
}