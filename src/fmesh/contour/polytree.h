#ifndef FMESH_POLYTREE_1604475054469_H
#define FMESH_POLYTREE_1604475054469_H
#include <clipper/clipper.hpp>
#include "trimesh2/Vec.h"

namespace fmesh
{
	typedef std::function<ClipperLib::IntPoint(const ClipperLib::IntPoint& point)> polyOffsetFunc;
	//all parameters are in mm
	void convertPaths2PolyTree(ClipperLib::Paths* paths, ClipperLib::PolyTree& polyTree);
	
	//extend
	void extendPolyTree(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest);
	void offsetPolyTree(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest);
	void extendPolyTree(ClipperLib::PolyTree& source, double delta, polyOffsetFunc offsetFunc, ClipperLib::PolyTree& dest);
	void offsetAndExtendPolyTree(ClipperLib::PolyTree& source, double offset, double delta, double z, ClipperLib::PolyTree& dest);

	//offset
	void polyTreeOffset(ClipperLib::PolyTree& source, polyOffsetFunc offsetFunc);

	//xor
	void xor2PolyTrees(ClipperLib::PolyTree* outer, ClipperLib::PolyTree* inner, ClipperLib::PolyTree& out);
}

#endif // FMESH_POLYTREE_1604475054469_H