#ifndef CDRDXF_LOAD_1605251140503_H
#define CDRDXF_LOAD_1605251140503_H
#include <clipper/clipper.hpp>
#include "fmesh/common/export.h"

namespace cdrdxf
{
	FMESH_API ClipperLib::Paths* loadDXFFile(const char* fileName);
	FMESH_API ClipperLib::Paths* loadSplineFromDXFFile(const char* fileName);
}

#endif // CDRDXF_LOAD_1605251140503_H