#ifndef FMESH_DVECUTIL_1604489834786_H
#define FMESH_DVECUTIL_1604489834786_H
#include "trimesh2/Vec.h"

namespace fmesh
{
	inline double crossValue(const trimesh::dvec3& v10, const trimesh::dvec3& v12)
	{
		return v10.x * v12.y - v10.y * v12.x;
	}

	inline double dotValue(const trimesh::dvec3& v10, const trimesh::dvec3& v12)
	{
		trimesh::dvec3 nv10 = trimesh::normalized(v10);
		trimesh::dvec3 nv12 = trimesh::normalized(v12);
		return trimesh::dot(nv10, nv12);
	}

	bool insideTriangle(const trimesh::dvec3& va, const trimesh::dvec3& vb, const trimesh::dvec3& vc,
		const trimesh::dvec3& vp);
	bool insideTriangleEx(const trimesh::dvec3& va, const trimesh::dvec3& vb, const trimesh::dvec3& vc,
		const trimesh::dvec3& vp);
}

#endif // FMESH_DVECUTIL_1604489834786_H