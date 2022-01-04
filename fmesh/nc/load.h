#ifndef NC_LOAD_1605251140503_H
#define NC_LOAD_1605251140503_H
#include "clipperxyz/clipper.hpp"
#include "fmesh/common/export.h"

namespace cdrdxf
{
	FMESH_API ClipperLibXYZ::Paths* loadNCFile(const char* fileName);
	FMESH_API std::vector<ClipperLibXYZ::Paths*> loadMultiNCFile(const char* fileName);
	FMESH_API ClipperLibXYZ::Paths* loadSplineFromNCFile(const char* fileName);

	int divisionXYZ(std::string str, ClipperLibXYZ::IntPoint& point);
}

#endif // NC_LOAD_1605251140503_H