#ifndef FMESH_ROOF_1605318972342_H
#define FMESH_ROOF_1605318972342_H
#include "fmesh/generate/patch.h"
#include <clipper/clipper.hpp>

namespace fmesh
{
	Patch* buildRoof(ClipperLib::PolyTree* polyTree, double roofHeight);
	void buildRoofs(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches, double roofHeight, double thickness);

	void roofLine(ClipperLib::PolyTree* polyTree, ClipperLib::PolyTree* roof);
}

#endif // FMESH_ROOF_1605318972342_H