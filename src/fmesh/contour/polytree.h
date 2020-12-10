#ifndef FMESH_POLYTREE_1604475054469_H
#define FMESH_POLYTREE_1604475054469_H
#include <clipper/clipper.hpp>
#include "trimesh2/Vec.h"
#include "fmesh/generate/patch.h"

namespace fmesh
{
	typedef std::function<ClipperLib::IntPoint(const ClipperLib::IntPoint& point)> polyOffsetFunc;
	//all parameters are in mm
	void convertPaths2PolyTree(ClipperLib::Paths* paths, ClipperLib::PolyTree& polyTree);
	void copy2PolyTree(ClipperLib::PolyTree& source, ClipperLib::PolyTree& dest);

	//extend
	void extendPolyTree(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest);
	void extendPolyTree(ClipperLib::PolyTree& source, double delta, polyOffsetFunc offsetFunc, ClipperLib::PolyTree& dest);
	void offsetAndExtendPolyTree(ClipperLib::PolyTree& source, double offset, double delta, double z, ClipperLib::PolyTree& dest);
	void offsetExterior(ClipperLib::PolyTree& source, double offset);
	void offsetExteriorInner(ClipperLib::PolyTree& source, double offset);

	//Polygon skeleton
	void skeletonPolyTree(ClipperLib::PolyTree& source, double z, std::vector<Patch*>& patches);

	//offset
	void polyTreeOffset(ClipperLib::PolyTree& source, polyOffsetFunc offsetFunc);
	void offsetPolyTree(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest);
	void offsetPaths(std::vector<ClipperLib::Path*>& source, double delta, ClipperLib::PolyTree& dest);
	void offsetPath(ClipperLib::Path* source, double delta, ClipperLib::PolyTree& dest);

	//xor
	void xor2PolyTrees(ClipperLib::PolyTree* outer, ClipperLib::PolyTree* inner, ClipperLib::PolyTree& out);

	//seperate
	struct PolyPair
	{
		bool clockwise;
		ClipperLib::PolyNode* outer;
		std::vector<ClipperLib::PolyNode*> inner;
	};
	void seperate1423(ClipperLib::PolyTree* polyTree, std::vector<PolyPair*>& polyPairs);
}

#endif // FMESH_POLYTREE_1604475054469_H