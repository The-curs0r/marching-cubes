#version 450 core
out vec4 color;
uniform sampler2D noise;
uniform vec3 lightPos = vec3(0.0f,0.0f,0.0f);
in VS_OUT{
	vec2 vUv;
	vec3 normal;
	vec3 worldPos;
}fs_in;
void main(void) {
	//vec2 vUv = fs_in.vUv;
	//vec2 uv = fs_in.vUv - 0.5;
	//color = vec4(vec3(fs_in.color), 1.0f);
	//color = vec4(1.0f,0.0f,0.0f, 1.0f);

	//color = vec4(vec3(uv.y+0.5), 1.0f);
	//float temp = -uv.y;
	//temp += texture(noise, vUv).x;
	//color = vec4(vec3(temp), 1.0f);
	//color = texture(noise, vUv);
	highp vec3 L = lightPos - fs_in.worldPos;
	highp float NL = max(dot(fs_in.normal, normalize(L)), 0.0);
	highp vec3 colort = mix(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), fs_in.worldPos.y);
	//highp vec3 colort = vec3(abs(1 * fs_in.normal[0]), abs(1 * fs_in.normal[1]), abs(1 * fs_in.normal[2])) / max(1.0, pow(length(L), 2));
	//highp vec3 colort = vec3(1.0f)/max(1.0,pow(length(L),2));
	color = vec4(colort * NL * 0.8, 1.0);

}