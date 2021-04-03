#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT{
	vec2 vUv;
}gs_in[3];

out GS_OUT{
	vec2 vUv;
	vec3 normal;
	vec3 worldPos;
}gs_out;

void main(void) {
	
	vec3 A = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 B = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 norm = normalize(cross(A, B));

	gs_out.normal = norm;
	gs_out.vUv = gs_in[0].vUv;
	gl_Position = gl_in[0].gl_Position;
	gs_out.worldPos = gl_Position.xyz;
	//gs_out.uv = vec2(1.0, 1.0);
	EmitVertex();

	gs_out.normal = norm;
	gs_out.vUv = gs_in[0].vUv;
	gl_Position = gl_in[1].gl_Position;
	gs_out.worldPos = gl_Position.xyz;
	//gs_out.uv = vec2(0.0, 1.0);
	EmitVertex();

	gs_out.normal = norm;
	gs_out.vUv = gs_in[0].vUv;
	gl_Position = gl_in[2].gl_Position;
	gs_out.worldPos = gl_Position.xyz;
	//gs_out.uv = vec2(0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}