#ifndef CDRDXF_WRITEDXF_1605518188021_H
#define CDRDXF_WRITEDXF_1605518188021_H
#include "fmesh/common/point.h"
#include "fmesh/common/export.h"
#include <clipper/clipper.hpp>

namespace cdrdxf
{
	FMESH_API void writedxf(ClipperLib::PolyTree* tree, const std::string& file);
	FMESH_API void writedxf(std::vector<ClipperLib::PolyTree*>& trees, const std::string& file);
}

#endif // CDRDXF_WRITEDXF_1605518188021_H