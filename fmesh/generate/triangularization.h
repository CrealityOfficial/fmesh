#ifndef FMESH_TRIANGULARIZATION_1604485277802_H
#define FMESH_TRIANGULARIZATION_1604485277802_H
#include "fmesh/common/export.h"
#include "fmesh/generate/patch.h"
#include "clipperxyz/clipper.hpp"

namespace fmesh
{
	bool checkFlag(ClipperLibXYZ::PolyNode* node, int flag);
	FMESH_API Patch* buildFromSamePolyTree(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, int flag = 0);
	
	void buildFromDiffPolyTree(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp,
		std::vector<Patch*>& patches, int flag = 0);

	void buildFromDiffPolyTree_SameAndDiffSafty(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp,
		std::vector<Patch*>& patches, int flag, ClipperLibXYZ::PolyTree& out, double delta);
	void buildFromDiffPolyTreeSafty(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp,
		std::vector<Patch*>& patches, double delta, int flag = 0);

	struct PolyTreeOppoPair
	{
		ClipperLibXYZ::PolyNode* lower;
		ClipperLibXYZ::PolyNode* upper;
	};

	FMESH_API void findPolyTreePairFromNode(ClipperLibXYZ::PolyNode* nodeLower, ClipperLibXYZ::PolyNode* nodeUp,
		std::vector<PolyTreeOppoPair>& pairs, double delta);

	Patch* buildFromDiffPath(ClipperLibXYZ::Path* pathLower, ClipperLibXYZ::Path* pathUp);
	void buildFromPathes(std::vector<ClipperLibXYZ::Path*>& pathsLower, std::vector<ClipperLibXYZ::Path*>& pathsUp, Patch& patch);
	void buildFromPath(ClipperLibXYZ::Path* pathLower, ClipperLibXYZ::Path* pathUp, Patch& patch);

	void buildXORFrom2PolyTree(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, ClipperLibXYZ::PolyTree& out, int flag = 0);

	inline trimesh::vec3 CInt2V(const ClipperLibXYZ::IntPoint& point)
	{
		return trimesh::vec3(INT2MM(point.X), INT2MM(point.Y), INT2MM(point.Z));
	}

	inline trimesh::dvec3 CInt2VD(const ClipperLibXYZ::IntPoint& point)
	{
		return trimesh::dvec3(INT2MM(point.X), INT2MM(point.Y), INT2MM(point.Z));
	}
}

#endif // FMESH_TRIANGULARIZATION_1604485277802_H