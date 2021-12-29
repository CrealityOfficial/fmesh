#ifndef CDRDXF_LOAD_1605253199928_H
#define CDRDXF_LOAD_1605253199928_H
#include <clipper/clipper.hpp>
#include "fmesh/common/export.h"

namespace cdrdxf
{
	FMESH_API std::vector<ClipperLib::Paths*> loadMultiSVGFile(const char* fileName);
	FMESH_API ClipperLib::Paths* loadSVGFile(const char* fileName);
}

#endif // CDRDXF_LOAD_1605253199928_H