#version 450 core

out vec4 color;

uniform sampler2D noise;
in GS_OUT{
	vec2 vUv;
	vec3 normal;
	vec3 worldPos;
}fs_in;

void main(void) {
	vec2 vUv = fs_in.vUv;
	vec2 uv = fs_in.vUv - 0.5;
	//color = vec4(vec3(fs_in.color), 1.0f);
	color = vec4(1.0f,0.0f,0.0f, 1.0f);

	//color = vec4(vec3(uv.y+0.5), 1.0f);
	//float temp = -uv.y;
	//temp += texture(noise, vUv).x;
	//color = vec4(vec3(temp), 1.0f);
	//color = texture(noise, vUv);

	highp vec3 L = normalize(vec3(2.0f,2.0f,0.0f) - fs_in.worldPos);
	highp float NL = max(dot(fs_in.normal, L), 0.0);
	highp vec3 colort = vec3(1, 1, 0.0);
	color = vec4(colort * NL, 1.0);

}