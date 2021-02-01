#ifndef CDRDXF_WRITESVG_1609317666015_H
#define CDRDXF_WRITESVG_1609317666015_H
#include <clipper/clipper.hpp>
#include "fmesh/common/export.h"

namespace cdrdxf
{
	FMESH_API void writesvg(ClipperLib::PolyTree* tree, const std::string& file);
	FMESH_API void writesvg(std::vector<ClipperLib::PolyTree*>& trees, const std::string& file);
}

#endif // CDRDXF_WRITESVG_1609317666015_H