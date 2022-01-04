#ifndef MMESH_CONTOUR_1603376914865_H
#define MMESH_CONTOUR_1603376914865_H
#include "clipperxyz/clipper.hpp"
#include "trimesh2/Vec.h"

namespace fmesh
{
	void fill(trimesh::vec3* vertex, ClipperLibXYZ::Path& path);
}

#endif // MMESH_CONTOUR_1603376914865_H