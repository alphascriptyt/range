#pragma once

#include "plane.h"
#include "vectormaths.h"
#include "triangle3D.h"

#include <vector>


class ViewFrustum {
	float zNear = 0.1f;
	std::vector<Plane> planes; // store the planes representing the view frustum

	void setViewFrustumPlanes();	// method to get the planes for the view frustum

	// clipping helper methods
	void clipTrianglesAgainstPlane(std::vector<Triangle3D>& triangles, Plane& plane);

public:
	// constructors
	ViewFrustum();
	ViewFrustum(float z_near);

	// methods
	int isMeshVisible(std::vector<Triangle3D>& triangles);	// determine if a mesh is within the frustum
	void clipTriangles(std::vector<Triangle3D>& triangles);	// actually clip triangles
};