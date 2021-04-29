#ifndef FMESH_TRIANGULARIZATION_1604485277802_H
#define FMESH_TRIANGULARIZATION_1604485277802_H
#include "fmesh/common/export.h"
#include "fmesh/generate/patch.h"
#include <clipper/clipper.hpp>

namespace fmesh
{
	bool checkFlag(ClipperLib::PolyNode* node, int flag);
	FMESH_API Patch* buildFromSamePolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag = 0);
	
	void buildFromDiffPolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp,
		std::vector<Patch*>& patches, int flag = 0);

	void buildFromDiffPolyTree_SameAndDiffSafty(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp,
		std::vector<Patch*>& patches, int flag, ClipperLib::PolyTree& out, double delta);
	void buildFromDiffPolyTreeSafty(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp,
		std::vector<Patch*>& patches, double delta, int flag = 0);

	void buildFromSameAndDiff(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp,
		std::vector<Patch*>& patches, int flag, double delta ,ClipperLib::PolyTree& out, ClipperLib::PolyTree& Inner);

	struct PolyTreeOppoPair
	{
		ClipperLib::PolyNode* lower;
		ClipperLib::PolyNode* upper;
	};

	FMESH_API void findPolyTreePairFromNode(ClipperLib::PolyNode* nodeLower, ClipperLib::PolyNode* nodeUp,
		std::vector<PolyTreeOppoPair>& pairs, double delta);

	Patch* buildFromDiffPath(ClipperLib::Path* pathLower, ClipperLib::Path* pathUp);
	void buildFromPathes(std::vector<ClipperLib::Path*>& pathsLower, std::vector<ClipperLib::Path*>& pathsUp, Patch& patch);
	void buildFromPath(ClipperLib::Path* pathLower, ClipperLib::Path* pathUp, Patch& patch);

	void buildXORFrom2PolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, ClipperLib::PolyTree& out, int flag = 0);

	inline trimesh::vec3 CInt2V(const ClipperLib::IntPoint& point)
	{
		return trimesh::vec3(INT2MM(point.X), INT2MM(point.Y), INT2MM(point.Z));
	}

	inline trimesh::dvec3 CInt2VD(const ClipperLib::IntPoint& point)
	{
		return trimesh::dvec3(INT2MM(point.X), INT2MM(point.Y), INT2MM(point.Z));
	}
}

#endif // FMESH_TRIANGULARIZATION_1604485277802_H