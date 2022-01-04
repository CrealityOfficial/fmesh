#ifndef CDRDXF_WRITESVG_1609317666015_H
#define CDRDXF_WRITESVG_1609317666015_H
#include "clipperxyz/clipper.hpp"
#include "fmesh/common/export.h"

namespace cdrdxf
{
	//FMESH_API void writesvg(ClipperLibXYZ::PolyTree* tree, const std::string& file);
	FMESH_API void writesvg(std::vector<ClipperLibXYZ::PolyTree*>& trees, const std::string& file);

	void getSVGAllPolyPaths(std::vector<ClipperLibXYZ::PolyTree*>& trees, std::vector<std::vector<ClipperLibXYZ::Path*>>& vecPaths);
	void processSVGData(std::vector<std::vector<ClipperLibXYZ::Path*>>& vecPaths,const std::string& file);
}

#endif // CDRDXF_WRITESVG_1609317666015_H