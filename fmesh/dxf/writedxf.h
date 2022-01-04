#ifndef CDRDXF_WRITEDXF_1605518188021_H
#define CDRDXF_WRITEDXF_1605518188021_H
#include "fmesh/common/point.h"
#include "fmesh/common/export.h"
#include "clipperxyz/clipper.hpp"

namespace cdrdxf
{
	//FMESH_API void writedxf(ClipperLibXYZ::PolyTree* tree, const std::string& file);
	FMESH_API void writedxf(std::vector<ClipperLibXYZ::PolyTree*>& trees, const std::string& file);
	void getDXFAllPolyPaths(std::vector<ClipperLibXYZ::PolyTree*>& trees, std::vector<std::vector<ClipperLibXYZ::Path*>>& paths);
	void processDXFData(std::vector<std::vector<ClipperLibXYZ::Path*>>& paths, const std::string& file);
}

#endif // CDRDXF_WRITEDXF_1605518188021_H