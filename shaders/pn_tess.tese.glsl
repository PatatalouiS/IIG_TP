#version 410 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4 view;
uniform mat4 projection;

struct ControlPoints
{
    // Original vertices
    vec3 B300;
    vec3 B030;
    vec3 B003;
    // Mid vertices
    vec3 B210;
    vec3 B120;
    vec3 B021;
    vec3 B012;
    vec3 B102;
    vec3 B201;
    // Center
    vec3 B111;

    //normals
    vec3 N200;
    vec3 N020;
    vec3 N002;
    vec3 N110;
    vec3 N011;
    vec3 N101;
};

patch in ControlPoints CP;

out vec3 worldPosition;
//out vec2 TexCoord_FS_in;
out vec3 Normal_FS_in;
out vec3 position;

/*
vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}
*/

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
    //TexCoord_FS_in = interpolate2D(CP.TexCoord[0], CP.TexCoord[1], CP.TexCoord[2]);

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;
    float uPow3 = pow(u, 3);
    float vPow3 = pow(v, 3);
    float wPow3 = pow(w, 3);
    float uPow2 = pow(u, 2);
    float vPow2 = pow(v, 2);
    float wPow2 = pow(w, 2);

    worldPosition = (CP.B300 * wPow3) + (CP.B030 * uPow3) + (CP.B003 * vPow3) +
                     (CP.B210 * 3.0 * wPow2 * u) + (CP.B120 * 3.0 * w * uPow2) + (CP.B201 * 3.0 * wPow2 * v) +
                     (CP.B021 * 3.0 * uPow2 * v) + (CP.B102 * 3.0 * w * vPow2) + (CP.B012 * 3.0 * u * vPow2) +
                     (CP.B111 * 6.0 * w * u * v);

    //Normal_FS_in = normalize((1 - u - v) * CP.N200 + u * CP.N020 + v * CP.N002);
    Normal_FS_in = normalize((CP.N200 * wPow2) + (CP.N020 * uPow2) + (CP.N002 * vPow2) +
                             (CP.N110 * w * u) + (CP.N011 * u * v) + (CP.N101 * w * v));

    gl_Position = projection * view * vec4(worldPosition, 1.0);
    position = gl_Position.xyz;
}
