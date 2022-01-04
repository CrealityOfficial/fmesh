#ifndef CDRDXF_LOAD_1605251140503_H
#define CDRDXF_LOAD_1605251140503_H
#include "clipperxyz/clipper.hpp"
#include "fmesh/common/export.h"

namespace cdrdxf
{
	FMESH_API ClipperLibXYZ::Paths* loadDXFFile(const char* fileName);
	FMESH_API std::vector<ClipperLibXYZ::Paths*> loadMultiDXFFile(const char* fileName);
	FMESH_API ClipperLibXYZ::Paths* loadSplineFromDXFFile(const char* fileName);
}

#endif // CDRDXF_LOAD_1605251140503_H