#include "widenoutergenerator.h"
#include "fmesh/generate/triangularization.h"

namespace fmesh
{
	WidenOuterGenerator::WidenOuterGenerator()
	{

	}

	WidenOuterGenerator::~WidenOuterGenerator()
	{

	}

	void WidenOuterGenerator::build()
	{
		m_adParam.bottom_type = ADBottomType::adbt_extend_outter;
		ClipperLib::PolyTree treeTop, treeBottom;
		double hTop, hBottom;

		_buildTop(treeTop, hTop);
		_buildBottom(treeBottom, hBottom);

		_buildFromSamePolyTree(&treeBottom, &treeTop);
	}

	void WidenOuterGenerator::buildShell()
	{
		m_adParam.bottom_type = ADBottomType::adbt_extend_outter;
		ClipperLib::PolyTree treeTop, treeBottom;
		double hTop, hBottom;

		_buildTop_onepoly(treeTop, hTop);
		_buildBottom_onepoly(treeBottom, hBottom);

		_buildFromSamePolyTree(&treeBottom, &treeTop);
	}

	void WidenOuterGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		m_adParam.bottom_type = ADBottomType::adbt_extend_outter;
		double hTop, hBottom;

		_buildTop_onepoly(topTree, hTop);
		_buildBottom_onepoly(bottomTree, hBottom);

// 		std::vector<double> area;
// 		areaPoly(topTree, area);
// 		_buildBoardPoly(&topTree);
// 		_buildBoardPoly(&bottomTree);
	}
}