#ifndef NC_LOAD_1605251140503_H
#define NC_LOAD_1605251140503_H
#include <clipper/clipper.hpp>
#include "fmesh/common/export.h"

namespace cdrdxf
{
	FMESH_API ClipperLib::Paths* loadNCFile(const char* fileName);
	FMESH_API std::vector<ClipperLib::Paths*> loadMultiNCFile(const char* fileName);
	FMESH_API ClipperLib::Paths* loadSplineFromNCFile(const char* fileName);

	int divisionXYZ(std::string str, ClipperLib::IntPoint& point);
}

#endif // NC_LOAD_1605251140503_H