#ifndef CDRDXF_LOAD_1605253199928_H
#define CDRDXF_LOAD_1605253199928_H
#include "clipperxyz/clipper.hpp"
#include "fmesh/common/export.h"

namespace cdrdxf
{
	FMESH_API std::vector<ClipperLibXYZ::Paths*> loadMultiSVGFile(const char* fileName);
	FMESH_API ClipperLibXYZ::Paths* loadSVGFile(const char* fileName);
}

#endif // CDRDXF_LOAD_1605253199928_H