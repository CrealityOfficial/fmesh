#ifndef CDRDXF_WRITEDXF_1605518188021_H
#define CDRDXF_WRITEDXF_1605518188021_H
#include "fmesh/common/point.h"
#include <clipper/clipper.hpp>
#include <vector>

namespace cdrdxf
{
	void writedxf(ClipperLib::PolyTree* tree, const std::string& file);
}

#endif // CDRDXF_WRITEDXF_1605518188021_H