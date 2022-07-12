#pragma once
#include "mat4D.h"
#include "vec3D.h"
#include "plane.h"

// TODO: WRITE COMMENTS

V3 multiplyBy4x1(M4& m, V3& v); // V3 can be interpreted as a 4x1 matrix by using w
V3 vectorCrossProduct(V3& v1, V3& v2);
float vectorDotProduct(V3& v1, V3& v2);
V3 vectorIntersectPlane(V3& v1, V3& v2, Plane& plane);
void rotateV3(V3& v, float pitch, float yaw);
float findSignedDistance(V3& v, Plane& plane);