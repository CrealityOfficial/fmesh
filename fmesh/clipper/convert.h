#ifndef MMESH_CONVERT_1625103040213_H
#define MMESH_CONVERT_1625103040213_H
#include "clipperxyz/clipper.hpp"
#include "trimesh2/Vec.h"

namespace fmesh
{
	void clipperConvert(const ClipperLibXYZ::Path& path, std::vector<trimesh::vec3>& vpath);
	void clipperConvert(const ClipperLibXYZ::Paths& paths, std::vector<trimesh::vec3>& vpath);
	void clipperConvert(const std::vector<ClipperLibXYZ::Paths*>& pathses, std::vector<trimesh::vec3>& vpath);
}

#endif // MMESH_CONVERT_1625103040213_H