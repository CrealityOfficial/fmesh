#ifndef FMESH_CONTOUR_1603287200866_H
#define FMESH_CONTOUR_1603287200866_H
#include "fmesh/common/export.h"
#include "clipperxyz/clipper.hpp"

namespace fmesh
{
	class Outline;
	ClipperLibXYZ::PolyTree* convertOutline2PolyTree(Outline* outline);
	ClipperLibXYZ::PolyTree* convertPaths2PolyTree(ClipperLibXYZ::Paths* paths);
	ClipperLibXYZ::PolyTree* convertPolyTrees2PolyTree(std::vector<ClipperLibXYZ::PolyTree*>& trees);
	ClipperLibXYZ::PolyTree* merge2PolyTrees(ClipperLibXYZ::PolyTree* outer, ClipperLibXYZ::PolyTree* inner);

	ClipperLibXYZ::PolyTree* extendPaths2PolyTree(ClipperLibXYZ::Paths* paths, float delta = 0.05);
	ClipperLibXYZ::PolyTree* extendPolyTree2PolyTree(ClipperLibXYZ::PolyTree* poly, float delta);

	ClipperLibXYZ::PolyTree* offsetPolyTree(ClipperLibXYZ::PolyTree* poly, float delta);

	void savePolyTree(ClipperLibXYZ::PolyTree* poly, const char* fileName);
	FMESH_API ClipperLibXYZ::PolyTree* loadPolyTree(const char* fileName);

	ClipperLibXYZ::PolyTree* offsetAndExtend(ClipperLibXYZ::PolyTree* poly, double offset, double extend);
}

#endif // FMESH_CONTOUR_1603287200866_H