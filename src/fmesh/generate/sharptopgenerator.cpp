#include "sharptopgenerator.h"
#include "fmesh/contour/contour.h"
#include "mmesh/clipper/circurlar.h"

namespace fmesh
{
	SharptopGenerator::SharptopGenerator()
	{

	}

	SharptopGenerator::~SharptopGenerator()
	{

	}

	void SharptopGenerator::build()
	{
		//test data
// 		m_adParam.top_type = ADTopType::adtt_step;
// 		m_adParam.top_height = 1.0;
// 		m_adParam.shape_top_height = 2.0;
// 		m_adParam.bottom_type = ADBottomType::adbt_step;
// 		m_adParam.bottom_height = 1.0;
// 		m_adParam.shape_bottom_height = 3.0;
		//

// 		double x = dmax.x - dmin.x;
// 		double y = dmax.y - dmin.y;
// 		double distance = x * y/10000 ? x * y / 10000 :1.415;

		double thickness = m_adParam.extend_width / 2.0;
		double bottomHeight = m_adParam.total_height - m_adParam.shape_top_height;

		//bottom
		std::vector<ClipperLib::PolyTree> bottomPolys(1);
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, bottomHeight, bottomPolys.at(0));

// 		polyNodeFunc func = [&distance](ClipperLib::PolyNode* node) {
// 			ClipperLib::CleanPolygon(node->Contour, distance);
// 		};
// 		mmesh::loopPolyTree(func, &bottomPolys.back());

		_simplifyPoly(&bottomPolys.back());

		std::vector<Patch*> patches;
		skeletonPolyTreeSharp(bottomPolys.back(), bottomHeight, m_adParam.shape_top_height, patches);
		addPatches(patches);

		_fillPolyTree(&bottomPolys.front(), true);
		_buildFromSamePolyTree(&bottomPolys.front(), &bottomPolys.back());

// 		ClipperLib::PolyTree treeBottom;
// 		double hBottom;
// 		_buildBottom(treeBottom, hBottom);
// 		_buildFromSamePolyTree(&treeBottom, &bottomPolys.front());

		_buildTopBottom(&bottomPolys.front(), nullptr);
	}
}