#version 450 core
layout(local_size_x = 1) in;
uniform vec3 stPoint;
uniform float inc;
const int numCubes = 32;
layout(binding = 0) writeonly buffer block1
{
    vec4 output_tri_data[numCubes * numCubes * numCubes * 15];
};
layout(binding = 1) readonly buffer block2
{
    int triTable[256][16];
};
//Simplex Noise from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
vec4 permute(vec4 x) { return mod(((x * 34.0) + 1.0) * x, 289.0); }
vec4 taylorInvSqrt(vec4 r) { return 1.79284291400159 - 0.85373472095314 * r; }
float snoise(vec3 v) {
    const vec2  C = vec2(1.0 / 6.0, 1.0 / 3.0);
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i = floor(v + dot(v, C.yyy));
    vec3 x0 = v - i + dot(i, C.xxx);

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min(g.xyz, l.zxy);
    vec3 i2 = max(g.xyz, l.zxy);

    //  x0 = x0 - 0. + 0.0 * C 
    vec3 x1 = x0 - i1 + 1.0 * C.xxx;
    vec3 x2 = x0 - i2 + 2.0 * C.xxx;
    vec3 x3 = x0 - 1. + 3.0 * C.xxx;

    // Permutations
    i = mod(i, 289.0);
    vec4 p = permute(permute(permute(
        i.z + vec4(0.0, i1.z, i2.z, 1.0))
        + i.y + vec4(0.0, i1.y, i2.y, 1.0))
        + i.x + vec4(0.0, i1.x, i2.x, 1.0));

    // Gradients
    // ( N*N points uniformly over a square, mapped onto an octahedron.)
    float n_ = 1.0 / 7.0; // N=7
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,N*N)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_);    // mod(j,N)

    vec4 x = x_ * ns.x + ns.yyyy;
    vec4 y = y_ * ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4(x.xy, y.xy);
    vec4 b1 = vec4(x.zw, y.zw);

    vec4 s0 = floor(b0) * 2.0 + 1.0;
    vec4 s1 = floor(b1) * 2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

    vec3 p0 = vec3(a0.xy, h.x);
    vec3 p1 = vec3(a0.zw, h.y);
    vec3 p2 = vec3(a1.xy, h.z);
    vec3 p3 = vec3(a1.zw, h.w);

    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
    m = m * m;
    return 42.0 * dot(m * m, vec4(dot(p0, x0), dot(p1, x1),
        dot(p2, x2), dot(p3, x3)));
}
int edgeTriangle[256] = { 0, 1 , 1 , 2 , 1 , 2 , 2 , 3 , 1 , 2 , 2 , 3 , 2 , 3 , 3 , 2 , 1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 , 2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 ,
1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 , 2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 , 2 , 3 , 3 , 2 , 3 , 4 , 4 , 3 , 3 , 4 , 4 , 3 , 4 , 5 , 5 , 2 ,
1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 , 2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 , 2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 , 3 , 4 , 4 , 5 , 4 , 5 , 5 , 4 ,
2 , 3 , 3 , 4 , 3 , 4 , 2 , 3 , 3 , 4 , 4 , 5 , 4 , 5 , 3 , 2 , 3 , 4 , 4 , 3 , 4 , 5 , 3 , 2 , 4 , 5 , 5 , 4 , 5 , 2 , 4 , 1 ,
1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 , 2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 , 2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 , 3 , 2 , 4 , 3 , 4 , 3 , 5 , 2 ,
2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 , 3 , 4 , 4 , 5 , 4 , 5 , 5 , 4 , 3 , 4 , 4 , 3 , 4 , 5 , 5 , 4 , 4 , 3 , 5 , 2 , 5 , 4 , 2 , 1 ,
2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 , 3 , 4 , 4 , 5 , 2 , 3 , 3 , 2 , 3 , 4 , 4 , 5 , 4 , 5 , 5 , 2 , 4 , 3 , 5 , 4 , 3 , 2 , 4 , 1 ,
3 , 4 , 4 , 5 , 4 , 5 , 3 , 4 , 4 , 5 , 5 , 2 , 3 , 4 , 2 , 1 , 2 , 3 , 3 , 2 , 3 , 4 , 2 , 1 , 3 , 2 , 4 , 1 , 2 , 1 , 1 ,  0 };
void main(void)
{
    vec3 coord = stPoint + vec3(gl_WorkGroupID.x * inc, gl_WorkGroupID.y * inc, gl_WorkGroupID.z * inc);
    float vertexNoise[8];
    float dim = inc;
    vec3 vertexCoord[8] = { vec3(coord.x, coord.y, coord.z),
                            vec3(coord.x, coord.y + dim, coord.z) ,
                            vec3(coord.x + dim, coord.y + dim, coord.z) ,
                            vec3(coord.x + dim, coord.y, coord.z) ,
                            vec3(coord.x, coord.y, coord.z + dim) ,
                            vec3(coord.x, coord.y + dim, coord.z + dim) ,
                            vec3(coord.x + dim, coord.y + dim, coord.z + dim) ,
                            vec3(coord.x + dim, coord.y, coord.z + dim) };
    for (int i = 0;i < 8;i++) {
        vertexNoise[i] = snoise(vertexCoord[i]);
    }
    int indexa = 0;
    for (int i = 0;i < 8;i++) {
        if (vertexNoise[i] > 0.0f) indexa += 1 << i;
    }
    vec3 edgeCoords[12];
    for (int i = 0;i < 12;i++) {
        if (i == 3) {
            edgeCoords[i] = ((vertexCoord[0] + vertexCoord[3]) / 2.0f);
            continue;
        }
        if (i == 7) {
            edgeCoords[i] = ((vertexCoord[4] + vertexCoord[7]) / 2.0f);
            continue;
        }
        if (i >= 8) {
            edgeCoords[i] = ((vertexCoord[i % 4] + vertexCoord[i % 4 + 4]) / 2.0f);
            continue;
        }
        edgeCoords[i] = ((vertexCoord[i % 8] + vertexCoord[(i + 1) % 8]) / 2.0f);
    }
    for (int i = 0;i < 15;i += 3) {
        if (i >= edgeTriangle[indexa] * 3) {
            output_tri_data[i + (gl_WorkGroupID.x * numCubes * numCubes + gl_WorkGroupID.y * numCubes + gl_WorkGroupID.z) * 15] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
            output_tri_data[i + 1 + (gl_WorkGroupID.x * numCubes * numCubes + gl_WorkGroupID.y * numCubes + gl_WorkGroupID.z) * 15] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
            output_tri_data[i + 2 + (gl_WorkGroupID.x * numCubes * numCubes + gl_WorkGroupID.y * numCubes + gl_WorkGroupID.z) * 15] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
            continue;
        }
        output_tri_data[i + (gl_WorkGroupID.x * numCubes * numCubes + gl_WorkGroupID.y * numCubes + gl_WorkGroupID.z) * 15] = vec4(1.0f, edgeCoords[triTable[indexa][i]]);
        output_tri_data[i + 1 + (gl_WorkGroupID.x * numCubes * numCubes + gl_WorkGroupID.y * numCubes + gl_WorkGroupID.z) * 15] = vec4(1.0f, edgeCoords[triTable[indexa][i+1]]);
        output_tri_data[i + 2 + (gl_WorkGroupID.x * numCubes * numCubes + gl_WorkGroupID.y * numCubes + gl_WorkGroupID.z) * 15] = vec4(1.0f, edgeCoords[triTable[indexa][i + 2]]);
    }
}