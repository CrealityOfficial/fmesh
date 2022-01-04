#ifndef FMESH_PATH_1604467840196_H
#define FMESH_PATH_1604467840196_H
#include "clipperxyz/clipper.hpp"
#include "fmesh/common/export.h"

namespace fmesh
{
	FMESH_API void pathBox(const ClipperLibXYZ::Path& path, ClipperLibXYZ::IntPoint& bmin, ClipperLibXYZ::IntPoint& bmax);
	FMESH_API void offsetPath(ClipperLibXYZ::Path& path, ClipperLibXYZ::IntPoint offset);
	FMESH_API void offsetPaths(ClipperLibXYZ::Paths& paths, ClipperLibXYZ::IntPoint offset);

	FMESH_API void scalePath2ExpectLen(ClipperLibXYZ::Paths* paths, double expectLen); //mm
	FMESH_API void calculatePathBox(ClipperLibXYZ::Paths* paths, ClipperLibXYZ::IntPoint& outMin, ClipperLibXYZ::IntPoint& outMax);

	FMESH_API void fixOrientation(std::vector<ClipperLibXYZ::Paths*>& pathses);
	FMESH_API void fixOrientation(ClipperLibXYZ::Paths* paths);
}

#endif // FMESH_PATH_1604467840196_H