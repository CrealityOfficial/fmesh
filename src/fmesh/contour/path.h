#ifndef FMESH_PATH_1604467840196_H
#define FMESH_PATH_1604467840196_H
#include <clipper/clipper.hpp>

namespace fmesh
{
	void scalePath2ExpectLen(ClipperLib::Paths* paths, double expectLen); //mm
	void calculatePathBox(ClipperLib::Paths* paths, ClipperLib::IntPoint& outMin, ClipperLib::IntPoint& outMax);
}

#endif // FMESH_PATH_1604467840196_H