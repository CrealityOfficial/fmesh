#ifndef FMESH_TRIANGULARIZATION_1604485277802_H
#define FMESH_TRIANGULARIZATION_1604485277802_H
#include "fmesh/generate/patch.h"
#include <clipper/clipper.hpp>

namespace fmesh
{
	bool checkFlag(ClipperLib::PolyNode* node, int flag);
	Patch* buildFromSamePolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag = 0);
	void buildFromDiffPolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp,
		std::vector<Patch*>& patches, int flag = 0);
	Patch* buildFromDiffPath(ClipperLib::Path* pathLower, ClipperLib::Path* pathUp);
	void buildFromPathes(std::vector<ClipperLib::Path*>& pathsLower, std::vector<ClipperLib::Path*>& pathsUp, Patch& patch);
	void buildFromPath(ClipperLib::Path* pathLower, ClipperLib::Path* pathUp, Patch& patch);

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