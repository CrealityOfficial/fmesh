#ifndef FMESH_POLYTREE_1604475054469_H
#define FMESH_POLYTREE_1604475054469_H
#include <clipper/clipper.hpp>
#include "fmesh/common/export.h"
#include "trimesh2/Vec.h"
#include "fmesh/generate/patch.h"

namespace fmesh
{
	typedef std::function<ClipperLib::IntPoint(const ClipperLib::IntPoint& point)> polyOffsetFunc;

	int testPolyNodeDepth(ClipperLib::PolyNode* node);
	void seperatePolyTree1234(ClipperLib::PolyTree* tree, std::vector<std::vector<ClipperLib::PolyNode*>>& depthNodes);

	//all parameters are in mm
	FMESH_API void convertPaths2PolyTree(ClipperLib::Paths* paths, ClipperLib::PolyTree& polyTree);
	FMESH_API void copy2PolyTree(ClipperLib::PolyTree& source, ClipperLib::PolyTree& dest);

	//extend
	FMESH_API void extendPolyTree(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest);
	FMESH_API void extendPolyTreeNew(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest);
	FMESH_API void extendPolyTreeMiter(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest);
	FMESH_API void extendPolyTree(ClipperLib::PolyTree& source, double delta, polyOffsetFunc offsetFunc, ClipperLib::PolyTree& dest);
	FMESH_API void offsetAndExtendPolyTree(ClipperLib::PolyTree& source, double offset, double delta, double z, ClipperLib::PolyTree& dest);
	FMESH_API void offsetExterior(ClipperLib::PolyTree& source, double offset);
	FMESH_API void offsetExteriorInner(ClipperLib::PolyTree& source, double offset);
	FMESH_API void offsetAndExtendPolyTree(ClipperLib::PolyTree& source, double offset, double delta, ClipperLib::PolyTree& dest);
	FMESH_API void offsetAndExtendPolyTreeMiter(ClipperLib::PolyTree& source, double offset, double delta, ClipperLib::PolyTree& dest);

	//set z
	FMESH_API void setPolyTreeZ(ClipperLib::PolyTree& tree, double z);
	FMESH_API void setPolyTreeZ(ClipperLib::PolyTree& tree, ClipperLib::cInt z);
	FMESH_API void adjustPolyTreeZ(ClipperLib::PolyTree& tree);

	//Polygon skeleton
	FMESH_API void skeletonPolyTree(ClipperLib::PolyTree& source, double z, std::vector<Patch*>& patches,double height, bool onePoly = false);
	//Polygon sharp 
	void skeletonPolyTreeSharp(ClipperLib::PolyTree& source, double z,double height, std::vector<Patch*>& patches,bool onePoly=false);

	//offset
	FMESH_API void polyTreeOffset(ClipperLib::PolyTree& source, polyOffsetFunc offsetFunc);
	FMESH_API void offsetPolyTree(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest);
	FMESH_API void offsetPolyTreeMiter(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest);
	FMESH_API void offsetPaths(std::vector<ClipperLib::Path*>& source, double delta, ClipperLib::PolyTree& dest);
	FMESH_API void offsetPath(ClipperLib::Path* source, double delta, ClipperLib::PolyTree& dest);
	FMESH_API void offsetPolyNodes(const std::vector<ClipperLib::PolyNode*>& polyNodes, double delta, ClipperLib::PolyTree& dest);
	//xor
	void xor2PolyTrees(ClipperLib::PolyTree* outer, ClipperLib::PolyTree* inner, ClipperLib::PolyTree& out);
	FMESH_API void xor2PolyTrees(ClipperLib::PolyTree* outer, ClipperLib::PolyTree* inner, ClipperLib::PolyTree& out, int flag);
	void xor2PolyNodes(const std::vector<ClipperLib::PolyNode*>& outer, 
		const std::vector<ClipperLib::PolyNode*>& inner, ClipperLib::PolyTree& out);

	//checkInnerOffset
	int GetPolyCount(ClipperLib::PolyTree* poly);

	FMESH_API void split(ClipperLib::PolyTree& source, std::vector<ClipperLib::Paths>& children);
	FMESH_API void split_omp(ClipperLib::PolyTree& source, std::vector<ClipperLib::Paths>& children);
}

#endif // FMESH_POLYTREE_1604475054469_H