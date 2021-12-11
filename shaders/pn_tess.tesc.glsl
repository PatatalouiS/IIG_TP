#version 410 core

// define the number of CPs in the output patch
layout (vertices = 1) out;

uniform int tessLevel;

// attributes of the input CPs
in vec3 worldPosition[];
in vec3 normal[];

vec3 P1 = worldPosition[0];
vec3 P2 = worldPosition[1];
vec3 P3 = worldPosition[2];

vec3 N1 = normal[0];
vec3 N2 = normal[1];
vec3 N3 = normal[2];

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

// attributes of the output CPs
patch out ControlPoints CP;

vec3 ProjectToPlane(vec3 Point, vec3 PlanePoint, vec3 PlaneNormal)
{
    vec3 v = Point - PlanePoint;
    float Len = dot(v, PlaneNormal);
    vec3 d = Len * PlaneNormal;
    return (Point - d);
}

void CalcPositions()
{
    // Original points
    CP.B300 = P1;
    CP.B030 = P2;
    CP.B003 = P3;

    // Mid points
    CP.B210 = (2 * P1 + P2 - dot(P2 - P1, N1) * N1) / 3.0;
    CP.B120 = (2 * P2 + P1 - dot(P1 - P2, N2) * N2) / 3.0;
    CP.B021 = (2 * P2 + P3 - dot(P3 - P2, N2) * N2) / 3.0;
    CP.B012 = (2 * P3 + P2 - dot(P2 - P3, N3) * N3) / 3.0;
    CP.B102 = (2 * P3 + P1 - dot(P1 - P3, N3) * N3) / 3.0;
    CP.B201 = (2 * P1 + P3 - dot(P3 - P1, N1) * N1) / 3.0;

    // Center point
    vec3 E = (CP.B210 + CP.B120 + CP.B021 + CP.B012 + CP.B102 + CP.B201) / 6.0;
    vec3 V = (P1 + P2 + P3) / 3.0;
    CP.B111 = E + (E - V) / 2.0;
}

void CalcNormals()
{
    CP.N200 = N1;
    CP.N020 = N2;
    CP.N002 = N3;

    vec3 D12 = (P2 - P1);
    vec3 D23 = (P3 - P2);
    vec3 D31 = (P1 - P3);

    float V12 = 2 * ((dot(D12, (N1 + N2))) / (dot(D12, D12)));
    float V23 = 2 * ((dot(D23, (N2 + N3))) / (dot(D23, D23)));
    float V31 = 2 * ((dot(D31, (N3 + N1))) / (dot(D31, D31)));

    CP.N110 = normalize(N1 + N2 - V12 * (P2 - P1));
    CP.N011 = normalize(N2 + N3 - V23 * (P3 - P2));
    CP.N101 = normalize(N3 + N1 - V31 * (P1 - P3));
}

void main()
{
    // Set the control points of the output patch
    CalcPositions();
    CalcNormals();

    // Calculate the tessellation levels
	gl_TessLevelOuter[0] = tessLevel;
	gl_TessLevelOuter[1] = tessLevel;
	gl_TessLevelOuter[2] = tessLevel;
	gl_TessLevelInner[0] = tessLevel;
}
