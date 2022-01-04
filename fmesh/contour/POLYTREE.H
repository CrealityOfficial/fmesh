#ifndef FMESH_POLYTREE_1604475054469_H
#define FMESH_POLYTREE_1604475054469_H
#include "clipperxyz/clipper.hpp"
#include "fmesh/common/export.h"
#include "trimesh2/Vec.h"
#include "fmesh/generate/patch.h"

namespace fmesh
{
	typedef std::function<ClipperLibXYZ::IntPoint(const ClipperLibXYZ::IntPoint& point)> polyOffsetFunc;

	int testPolyNodeDepth(ClipperLibXYZ::PolyNode* node);
	void seperatePolyTree1234(ClipperLibXYZ::PolyTree* tree, std::vector<std::vector<ClipperLibXYZ::PolyNode*>>& depthNodes);

	//all parameters are in mm
	FMESH_API void convertPaths2PolyTree(ClipperLibXYZ::Paths* paths, ClipperLibXYZ::PolyTree& polyTree);
	FMESH_API void copy2PolyTree(ClipperLibXYZ::PolyTree& source, ClipperLibXYZ::PolyTree& dest);

	//extend
	FMESH_API void extendPolyTree(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void extendPolyTreeNew(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void extendPolyTreeMiter(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void extendPolyTree(ClipperLibXYZ::PolyTree& source, double delta, polyOffsetFunc offsetFunc, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void offsetAndExtendPolyTreeNew(ClipperLibXYZ::PolyTree& source, double offset, double delta, double z, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void offsetAndExtendPolyTree(ClipperLibXYZ::PolyTree& source, double offset, double delta, double z, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void offsetAndExtendPolyTreeMiter(ClipperLibXYZ::PolyTree& source, double offset, double delta, double z, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void offsetExterior(ClipperLibXYZ::PolyTree& source, double offset,double z);
	FMESH_API void offsetExteriorInner(ClipperLibXYZ::PolyTree& source, double offset, double z);
	FMESH_API void offsetAndExtendPolyTree(ClipperLibXYZ::PolyTree& source, double offset, double delta, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void offsetAndExtendPolyTreeMiter(ClipperLibXYZ::PolyTree& source, double offset, double delta, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void extendPolyTreeOpen(ClipperLibXYZ::Paths& paths, double delta, ClipperLibXYZ::PolyTree& dest);

	//blunt sharp
	FMESH_API void offsetAndExtendpolyType(ClipperLibXYZ::PolyTree& source, double offset, double delta, double z, ClipperLibXYZ::PolyTree& dest,bool isBluntSharp);

	//set z
	FMESH_API void setPolyTreeZ(ClipperLibXYZ::PolyTree& tree, double z);
	FMESH_API void setPolyTreeZ(ClipperLibXYZ::PolyTree& tree, ClipperLibXYZ::cInt z);
	FMESH_API void adjustPolyTreeZ(ClipperLibXYZ::PolyTree& tree);

	//Polygon skeleton
	FMESH_API void skeletonPolyTree(ClipperLibXYZ::PolyTree& source, double z, std::vector<Patch*>& patches,double height, bool onePoly = false);
	double skeletonPoly(ClipperLibXYZ::PolyTree& source, ClipperLibXYZ::PolyTree& poly, double thickness);
	//Polygon sharp 
	void skeletonPolyTreeSharp(ClipperLibXYZ::PolyTree& source, double z,double height, std::vector<Patch*>& patches,bool onePoly=false);

	//offset
	FMESH_API void polyTreeOffset(ClipperLibXYZ::PolyTree& source, polyOffsetFunc offsetFunc);
	FMESH_API void offsetPolyTree(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void passivationPolyTree(ClipperLibXYZ::PolyTree& source, double delta);
	FMESH_API void offsetPolyTreeMiter(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void offsetPaths(std::vector<ClipperLibXYZ::Path*>& source, double delta, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void offsetPath(ClipperLibXYZ::Path* source, double delta, ClipperLibXYZ::PolyTree& dest);
	FMESH_API void offsetPolyNodes(const std::vector<ClipperLibXYZ::PolyNode*>& polyNodes, double delta, ClipperLibXYZ::PolyTree& dest);
	
	//blunt sharp
	FMESH_API void offsetPolyType(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest, bool isBluntSharp);
	
	//xor
	void xor2PolyTrees(ClipperLibXYZ::PolyTree* outer, ClipperLibXYZ::PolyTree* inner, ClipperLibXYZ::PolyTree& out);
	FMESH_API void xor2PolyTrees(ClipperLibXYZ::PolyTree* outer, ClipperLibXYZ::PolyTree* inner, ClipperLibXYZ::PolyTree& out, int flag);
	void xor2PolyNodes(const std::vector<ClipperLibXYZ::PolyNode*>& outer, 
		const std::vector<ClipperLibXYZ::PolyNode*>& inner, ClipperLibXYZ::PolyTree& out);

	//checkInnerOffset
	int GetPolyCount(ClipperLibXYZ::PolyTree* poly,int flag=0);//2: Inner  3: Outer
	double GetPolyArea(ClipperLibXYZ::PolyTree* poly, int flag = 0);//2: Inner  3: Outer
	ClipperLibXYZ::IntPoint getAABBvalue(ClipperLibXYZ::PolyTree* poly, int flag = 0);

	FMESH_API void split(ClipperLibXYZ::PolyTree& source, std::vector<ClipperLibXYZ::Paths>& children);
	FMESH_API void split_omp(ClipperLibXYZ::PolyTree& source, std::vector<ClipperLibXYZ::Paths>& children);
}

#endif // FMESH_POLYTREE_1604475054469_H