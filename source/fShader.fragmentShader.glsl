#version 450 core
out vec4 color;
uniform vec3 lightPos = vec3(0.0f,0.0f,0.0f);
uniform vec3 cameraDir = vec3(0.0f, 0.0f, 0.0f);
in VS_OUT{
	vec2 vUv;
	vec3 normal;
	vec3 worldPos;
}fs_in;

//Fog effect from https://www.iquilezles.org/www/articles/fog/fog.htm
vec3 applyFog(in vec3  rgb,      // original color of the pixel
	in float distance, // camera to point distance
	in vec3  rayOri,   // camera position
	in vec3  rayDir)  // camera to point vector
{
	float c = 0.3f;
	float b = 0.9f;
	float fogAmount = c * exp(-rayOri.y * b) * (1.0 - exp(-distance * rayDir.y * b)) / rayDir.y;
	vec3  fogColor = vec3(0.5, 0.6, 0.7);
	return mix(rgb, fogColor, fogAmount);
}

void main(void) {
	//vec2 vUv = fs_in.vUv;
	//vec2 uv = fs_in.vUv - 0.5;
	//color = vec4(vec3(uv.y+0.5), 1.0f);
	//float temp = -uv.y;
	//temp += texture(noise, vUv).x;
	//color = vec4(vec3(temp), 1.0f);
	//color = texture(noise, vUv);
	highp vec3 L = lightPos - fs_in.worldPos;
	highp float NL = max(dot(fs_in.normal, normalize(L)), 0.0);
	highp vec3 colort = mix(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), fs_in.worldPos.y) / max(1.0, pow(length(L), 2));
	color = vec4(applyFog(colort * NL * 0.8, length(L), lightPos, cameraDir), 1.0);
}