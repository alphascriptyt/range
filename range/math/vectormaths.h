#pragma once
#include "mat4D.h"
#include "vec3D.h"

// TODO: WRITE COMMENTS

//M4 multiplyByM4(M4& m2);
V3 multiplyBy4x1(M4& m, V3& v); // V3 can be interpreted as a 4x1 matrix
V3 vectorAdd(V3& v1, V3& v2);
void vectorAddTo(V3& v1, V3& v2);
V3 vectorSub(V3& v1, V3& v2);
V3 vectorMult(V3& v1, V3& v2);
void vectorMultBy(V3& v1, V3& v2);
V3 vectorCrossProduct(V3& v1, V3& v2);
float vectorDotProduct(V3& v1, V3& v2);
V3 vectorIntersectPlane(V3& v1, V3& v2, V3& plane_normal, V3& plane_point);
void rotateV3(V3& v, float pitch, float yaw);
