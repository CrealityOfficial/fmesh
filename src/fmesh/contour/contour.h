#ifndef FMESH_CONTOUR_1603287200866_H
#define FMESH_CONTOUR_1603287200866_H
#include "fmesh/common/export.h"
#include <clipper/clipper.hpp>

namespace fmesh
{
	class Outline;
	ClipperLib::PolyTree* convertOutline2PolyTree(Outline* outline);
	ClipperLib::PolyTree* convertPaths2PolyTree(ClipperLib::Paths* paths);
	ClipperLib::PolyTree* convertPolyTrees2PolyTree(std::vector<ClipperLib::PolyTree*>& trees);
	ClipperLib::PolyTree* merge2PolyTrees(ClipperLib::PolyTree* outer, ClipperLib::PolyTree* inner);

	ClipperLib::PolyTree* extendPaths2PolyTree(ClipperLib::Paths* paths, float delta = 0.05);
	ClipperLib::PolyTree* extendPolyTree2PolyTree(ClipperLib::PolyTree* poly, float delta);

	ClipperLib::PolyTree* offsetPolyTree(ClipperLib::PolyTree* poly, float delta);

	void savePolyTree(ClipperLib::PolyTree* poly, const char* fileName);
	FMESH_API ClipperLib::PolyTree* loadPolyTree(const char* fileName);

	ClipperLib::PolyTree* offsetAndExtend(ClipperLib::PolyTree* poly, double offset, double extend);
}

#endif // FMESH_CONTOUR_1603287200866_H