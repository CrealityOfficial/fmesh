#ifndef FMESH_POLYFILLER_1604488905380_H
#define FMESH_POLYFILLER_1604488905380_H
#include "fmesh/generate/earpolygon.h"

namespace fmesh
{
	Patch* fillSimplePolyTree(SimplePoly* poly);
	Patch* fillSimplePolyTree(ClipperLib::Path* path);

	void fillComplexPolyTree(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches);
	void fillComplexPolyTreeReverseInner(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches);

	Patch* fillOneLevelPolyNode(ClipperLib::PolyNode* polyNode, bool invert = false);
	void fillFirstLevelPolyNode(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches);
	void fillPolyNodeInner(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches);

	void fillPolyNodeOutline(ClipperLib::PolyTree* polyTree1, std::vector<Patch*>& patches);

	void merge2SimplePoly(ClipperLib::PolyNode* polyNode, SimplePoly* poly, bool invert);
	ClipperLib::cInt pathMaxX(ClipperLib::Path& path);
}

#endif // FMESH_POLYFILLER_1604488905380_H