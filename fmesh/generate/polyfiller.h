#ifndef FMESH_POLYFILLER_1604488905380_H
#define FMESH_POLYFILLER_1604488905380_H
#include "fmesh/common/export.h"
#include "fmesh/generate/earpolygon.h"

namespace fmesh
{
	FMESH_API Patch* fillSimplePolyTree(SimplePoly* poly);
	FMESH_API Patch* fillSimplePolyTree(ClipperLibXYZ::Path* path);
	FMESH_API Patch* fillSimplePolyTree(ClipperLibXYZ::PolyNode* poly);

	FMESH_API void fillComplexPolyTree(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches, bool muesEven = true);
	FMESH_API void fillComplexPolyTreeReverseInner(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches, bool invert=false);
	FMESH_API void fillComplexPolyTreeReverse(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches, bool isOuter = false);
	FMESH_API void fillComplexPolyTreeReverseInnerNew(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches, bool invert = false);
	FMESH_API void fillComplexPolyTree_onePloy(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches, bool invert = false);
	FMESH_API Patch* fillOneLevelPolyNode(ClipperLibXYZ::PolyNode* polyNode, bool invert = false);
	FMESH_API void fillFirstLevelPolyNode(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches);
	FMESH_API void fillPolyNodeInner(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches);
	FMESH_API void fillPolyTreeDepth14(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches);
	FMESH_API void fillPolyTreeDepth23(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches);
	FMESH_API void fillPolyTreeDepthOnePoly(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches);

	FMESH_API void fillPolyNodeOutline(ClipperLibXYZ::PolyTree* polyTree1, std::vector<Patch*>& patches);

	FMESH_API void dealPolyTreeAxisZ(ClipperLibXYZ::PolyTree* polyTree, double slope, double min, double height);

	FMESH_API void merge2SimplePoly(ClipperLibXYZ::PolyNode* polyNode, SimplePoly* poly, bool invert);
	FMESH_API ClipperLibXYZ::cInt pathMaxX(ClipperLibXYZ::Path& path);
	FMESH_API ClipperLibXYZ::cInt pathMaxZ(ClipperLibXYZ::Path& path);
	FMESH_API ClipperLibXYZ::cInt pathMaxZ(ClipperLibXYZ::PolyNode* node);
	
}

#endif // FMESH_POLYFILLER_1604488905380_H