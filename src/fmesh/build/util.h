#ifndef FMESH_UTIL_1604372843539_H
#define FMESH_UTIL_1604372843539_H
#include <clipper/clipper.hpp>

namespace fmesh
{
	void calculateBox(ClipperLib::PolyTree* poly, ClipperLib::IntPoint& bMin, ClipperLib::IntPoint& bMax);
}

#endif // FMESH_UTIL_1604372843539_H