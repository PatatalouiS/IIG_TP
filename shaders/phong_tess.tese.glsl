#version 410 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4 view;
uniform mat4 projection;

struct OutputPatch {
	// Control points
	vec3 PI;
	vec3 PJ;
	vec3 PK;
	// Normals
	vec3 NI;
	vec3 NJ;
	vec3 NK;
	// projection terms (pre - computed)
	vec3 PI_I_J;
	vec3 PI_J_K;
	vec3 PI_K_I;
};

patch in OutputPatch CP;

out vec3 worldPosition_final;
//out vec2 TexCoord_FS_in;
out vec3 normal_final;
out vec3 position_final;


void main() {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	float w = gl_TessCoord.z;

	float uPow2 = pow(u, 2);
	float vPow2 = pow(v, 2);
	float wPow2 = pow(w, 2);

	// compute value with Control points
	worldPosition_final = (uPow2 * CP.PI) + (vPow2 * CP.PJ) + (wPow2 * CP.PK) +
	(u * v * CP.PI_I_J) + (v * w * CP.PI_J_K) + (w * u * CP.PI_K_I);

	// Phong normal interpolation
	normal_final = normalize((u * CP.NI) + (v * CP.NJ) + (w * CP.NK));

	gl_Position = projection * view * vec4(worldPosition_final, 1.0);
	position_final = gl_Position.xyz;
}
