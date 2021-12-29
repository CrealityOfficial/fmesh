#include "dvecutil.h"

namespace fmesh
{
	bool insideTriangle(const trimesh::dvec3& va, const trimesh::dvec3& vb, const trimesh::dvec3& vc,
		const trimesh::dvec3& vp)
	{
		double ax = vc.x - vb.x; double ay = vc.y - vb.y;
		double bx = va.x - vc.x; double by = va.y - vc.y;
		double cx = vb.x - va.x; double cy = vb.y - va.y;
		double apx = vp.x - va.x; double apy = vp.y - va.y;
		double bpx = vp.x - vb.x; double bpy = vp.y - vb.y;
		double cpx = vp.x - vc.x; double cpy = vp.y - vc.y;

		double aCbp = ax * bpy - ay * bpx;
		double cCap = cx * apy - cy * apx;
		double bCcp = bx * cpy - by * cpx;
		return ((aCbp > 0.0) && (bCcp > 0.0) && (cCap > 0.0));
	}

	bool insideTriangleEx(const trimesh::dvec3& va, const trimesh::dvec3& vb, const trimesh::dvec3& vc,
		const trimesh::dvec3& vp)
	{
		double ax = vc.x - vb.x; double ay = vc.y - vb.y;
		double bx = va.x - vc.x; double by = va.y - vc.y;
		double cx = vb.x - va.x; double cy = vb.y - va.y;
		double apx = vp.x - va.x; double apy = vp.y - va.y;
		double bpx = vp.x - vb.x; double bpy = vp.y - vb.y;
		double cpx = vp.x - vc.x; double cpy = vp.y - vc.y;

		double aCbp = ax * bpy - ay * bpx;
		double cCap = cx * apy - cy * apx;
		double bCcp = bx * cpy - by * cpx;
		return ((aCbp > 0.0) && (bCcp > 0.0) && (cCap > 0.0));
	}
}