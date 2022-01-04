#ifndef MMESH_CIRCURLAR_1603379885432_H
#define MMESH_CIRCURLAR_1603379885432_H
#include <clipperxyz/clipper.hpp>

namespace fmesh
{
	void loopPolyTree(polyNodeFunc func, ClipperLibXYZ::PolyNode* polyNode);
	void level2PolyNode(polyNodeFunc func, ClipperLibXYZ::PolyNode* polyNode);
	void seperatePolyTree(ClipperLibXYZ::PolyTree* polyTree,
		std::vector<ClipperLibXYZ::Path*>& exterior, std::vector<ClipperLibXYZ::Path*>& interior);

	void seperatePolyTree(ClipperLibXYZ::PolyTree* polyTree,
		std::vector<ClipperLibXYZ::Path*>& exterior, std::vector<ClipperLibXYZ::Path*>& interior,
		std::vector<int>& iexterior, std::vector<int>& iinterior);
}

#endif // MMESH_CIRCURLAR_1603379885432_H