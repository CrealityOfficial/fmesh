#ifndef FMESH_POLYFILLER_1604488905380_H
#define FMESH_POLYFILLER_1604488905380_H
#include "fmesh/common/export.h"
#include "fmesh/generate/earpolygon.h"

namespace fmesh
{
	FMESH_API Patch* fillSimplePolyTree(SimplePoly* poly);
	FMESH_API Patch* fillSimplePolyTree(ClipperLib::Path* path);

	FMESH_API void fillComplexPolyTree(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches);
	FMESH_API void fillComplexPolyTreeReverseInner(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches, bool invert=false);

	FMESH_API Patch* fillOneLevelPolyNode(ClipperLib::PolyNode* polyNode, bool invert = false);
	FMESH_API void fillFirstLevelPolyNode(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches);
	FMESH_API void fillPolyNodeInner(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches);
	FMESH_API void fillPolyTreeDepth14(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches);

	FMESH_API void fillPolyNodeOutline(ClipperLib::PolyTree* polyTree1, std::vector<Patch*>& patches);

	FMESH_API void dealPolyTreeAxisZ(ClipperLib::PolyTree* polyTree, double slope, double min, double height);

	FMESH_API void merge2SimplePoly(ClipperLib::PolyNode* polyNode, SimplePoly* poly, bool invert);
	FMESH_API ClipperLib::cInt pathMaxX(ClipperLib::Path& path);
}

#endif // FMESH_POLYFILLER_1604488905380_H