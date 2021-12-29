#ifndef FMESH_PATH_1604467840196_H
#define FMESH_PATH_1604467840196_H
#include <clipper/clipper.hpp>
#include "fmesh/common/export.h"

namespace fmesh
{
	FMESH_API void pathBox(const ClipperLib::Path& path, ClipperLib::IntPoint& bmin, ClipperLib::IntPoint& bmax);
	FMESH_API void offsetPath(ClipperLib::Path& path, ClipperLib::IntPoint offset);
	FMESH_API void offsetPaths(ClipperLib::Paths& paths, ClipperLib::IntPoint offset);

	FMESH_API void scalePath2ExpectLen(ClipperLib::Paths* paths, double expectLen); //mm
	FMESH_API void calculatePathBox(ClipperLib::Paths* paths, ClipperLib::IntPoint& outMin, ClipperLib::IntPoint& outMax);

	FMESH_API void fixOrientation(std::vector<ClipperLib::Paths*>& pathses);
	FMESH_API void fixOrientation(ClipperLib::Paths* paths);
}

#endif // FMESH_PATH_1604467840196_H