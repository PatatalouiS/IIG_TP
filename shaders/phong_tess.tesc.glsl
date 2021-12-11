#version 410 core

layout (vertices = 1) out;
// we can optimize more if we set vertices = 3 and use gl_InvocationID ?

uniform int tessLevel;

in vec3 worldPosition[];
in vec3 normal[];

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

patch out OutputPatch CP;

// Do the orthogonal_projection of point Q on the plane defined
// by the point P and the normal N
vec3 orthogonal_projection(vec3 q, vec3 p, vec3 n) {
	 return q - (dot(q - p, n)) * n;
}

void main()
{
	// Our control points are just the triangle with normals
	CP.PI = worldPosition[0];
	CP.PJ = worldPosition[1];
	CP.PK = worldPosition[2];

	CP.NI = normal[0];
	CP.NJ = normal[1];
	CP.NK = normal[2];

	// Precompute the projection terms of the paper formula
	CP.PI_I_J = orthogonal_projection(CP.PJ, CP.PI, CP.NI) + orthogonal_projection(CP.PI, CP.PJ, CP.NJ);
	CP.PI_J_K = orthogonal_projection(CP.PK, CP.PJ, CP.NJ) + orthogonal_projection(CP.PJ, CP.PK, CP.NK);
	CP.PI_K_I = orthogonal_projection(CP.PI, CP.PK, CP.NK) + orthogonal_projection(CP.PK, CP.PI, CP.NI);

	// set the tessellationLevel
	gl_TessLevelOuter[0] = tessLevel;
	gl_TessLevelOuter[1] = tessLevel;
	gl_TessLevelOuter[2] = tessLevel;
	gl_TessLevelInner[0] = tessLevel;
}
