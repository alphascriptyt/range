#include "frustumculling.h"
#include "vectormaths.h"

// setup
ViewFrustum::ViewFrustum() {
	setViewFrustumPlanes();
}

ViewFrustum::ViewFrustum(float z_near) {
	zNear = z_near;
	setViewFrustumPlanes();
}

// methods
void ViewFrustum::setViewFrustumPlanes() {
	// define the near plane
	V3 near_normal = V3(0, 0, 1);
	V3 near_point = V3(0, 0, zNear);
	Plane near(near_normal, near_point);

	// define other planes
	V3 left_normal(-1, 0, 1);
	left_normal.normalize();
	Plane left(left_normal, near_point);

	V3 right_normal(1, 0, 1);
	right_normal.normalize();
	Plane right(right_normal, near_point);

	V3 top_normal(0, -1, 1);
	top_normal.normalize();
	Plane top(top_normal, near_point);

	V3 bottom_normal(0, 1, 1);
	bottom_normal.normalize();
	Plane bottom(bottom_normal, near_point);

	std::vector<Plane> frustum_planes;
	frustum_planes.push_back(near);
	frustum_planes.push_back(left);	// TODO: left and right are still SLIGHTLY messing up.	
	frustum_planes.push_back(right);
	frustum_planes.push_back(top);
	frustum_planes.push_back(bottom);
	
	planes = frustum_planes;
}

void ViewFrustum::clipTrianglesAgainstPlane(std::vector<Triangle3D>& triangles, Plane& plane) {
	std::vector<Triangle3D> clipped;

	for (auto& tri : triangles) {
		// store the points inside and outside of the near plane
		int inside_points_count = 0;
		int outside_points_count = 0;
		V3 inside_points[3];
		V3 outside_points[3];

		// calculate the distance away from the near plane
		float d1 = findSignedDistance(tri.v1, plane);
		float d2 = findSignedDistance(tri.v2, plane);
		float d3 = findSignedDistance(tri.v3, plane);

		bool in_first = false;
		bool in_second = false;

		Colour colour = tri.colour;

		// determine which points are inside or outside of the plane
		if (d1 >= 0) {
			inside_points[inside_points_count] = tri.v1;
			inside_points_count++;
			in_first = true;
		}
		else {
			outside_points[outside_points_count] = tri.v1;
			outside_points_count++;
		}
		if (d2 >= 0) {
			inside_points[inside_points_count] = tri.v2;
			inside_points_count++;
			in_second = true;
		}
		else {
			outside_points[outside_points_count] = tri.v2;
			outside_points_count++;
		}
		if (d3 >= 0) {
			inside_points[inside_points_count] = tri.v3;
			inside_points_count++;
		}
		else {
			outside_points[outside_points_count] = tri.v3;
			outside_points_count++;
		}

		// clip vertices
		if (inside_points_count == 3) {
			// if all points are inside of the plane we can just
			// return the original Triangle3D
			//Triangle3D tri = Triangle3D(tri.v1, tri.v2, tri.v3);
			clipped.push_back(tri);
		}
		else if (inside_points_count == 1 && outside_points_count == 2) {
			// if only one vertex is inside the Triangle3D
			// a new Triangle3D will be formed using the plane edge

			V3 nv1 = vectorIntersectPlane(inside_points[0], outside_points[0], plane);
			V3 nv2 = vectorIntersectPlane(inside_points[0], outside_points[1], plane);

			if (in_first) {
				Triangle3D tri = Triangle3D(inside_points[0], nv1, nv2);
				tri.colour = colour;
				clipped.push_back(tri);
			}
			else {
				if (in_second) {
					Triangle3D tri = Triangle3D(nv1, inside_points[0], nv2);
					tri.colour = colour;
					clipped.push_back(tri);
				}
				else {
					Triangle3D tri = Triangle3D(nv1, nv2, inside_points[0]);
					tri.colour = colour;
					clipped.push_back(tri);
				}
			}
		}

		else if (inside_points_count == 2 && outside_points_count == 1) {
			// if only two vertices are inside the plane
			// a quad will be formed and therefore two
			// new Triangle3Ds will be formed
			V3 nv1 = vectorIntersectPlane(inside_points[0], outside_points[0], plane);
			V3 nv2 = vectorIntersectPlane(inside_points[1], outside_points[0], plane);

			if (in_first) {
				if (in_second) {
					Triangle3D tri1 = Triangle3D(inside_points[0], inside_points[1], nv1);
					tri1.colour = colour;
					clipped.push_back(tri1);

					Triangle3D tri2 = Triangle3D(nv1, inside_points[1], nv2);
					tri2.colour = colour;
					clipped.push_back(tri2);
				}
				else {
					Triangle3D tri1 = Triangle3D(inside_points[0], nv1, inside_points[1]);
					tri1.colour = colour;
					clipped.push_back(tri1);

					Triangle3D tri2 = Triangle3D(nv1, nv2, inside_points[1]);
					tri2.colour = colour;
					clipped.push_back(tri2);
				}

			}
			else {
				Triangle3D tri1 = Triangle3D(nv1, inside_points[0], inside_points[1]);
				tri1.colour = colour;
				clipped.push_back(tri1);

				Triangle3D tri2 = Triangle3D(nv1, inside_points[1], nv2);
				tri2.colour = colour;
				clipped.push_back(tri2);
			}

		}
	}

	triangles = clipped;
}

void ViewFrustum::clipTriangles(std::vector<Triangle3D>& triangles) {
	// perform frustum culling
	for (auto& plane : planes) {
		clipTrianglesAgainstPlane(triangles, plane);
	}
}

int ViewFrustum::isMeshVisible(std::vector<Triangle3D>& triangles) {
	V3 center;

	for (auto& tri : triangles) {
		center += tri.v1 + tri.v2 + tri.v3;
	}

	float count = triangles.size() * 3;

	center /= count;

	// find farthest point
	float radius_squared = 0;
	for (auto& tri : triangles) {
		V3 line = tri.v1 - center;
		float size1 = line.sizeSquared();
		line = tri.v2 - center;
		float size2 = line.sizeSquared();
		line = tri.v3 - center;
		float size3 = line.sizeSquared();

		// sort size1 to be the largest
		if (size1 < size2) { std::swap(size1, size2); }
		if (size1 < size3) { std::swap(size1, size3); }

		if (size1 > radius_squared) {
			radius_squared = size1;
		}

	}

	float radius = sqrtf(radius_squared);

	for (auto& plane : planes) {
		// calculate distance from center of bounding sphere to plane
		float distance = findSignedDistance(center, plane);

		// distance < -radius = fully outside
		if (distance < -radius) {
			return 1;
		}
		// mesh intersects plane
		else if (distance < radius) {
			return 2;
		}
	}

	// distance > radius = fully inside
	return 0;
}
