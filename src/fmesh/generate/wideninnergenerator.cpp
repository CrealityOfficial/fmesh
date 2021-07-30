#include "wideninnergenerator.h"
#include "fmesh/generate/triangularization.h"

namespace fmesh
{
	WidenInnerGenerator::WidenInnerGenerator()
	{

	}

	WidenInnerGenerator::~WidenInnerGenerator()
	{

	}

	void WidenInnerGenerator::build()
	{
		m_adParam.bottom_type = ADBottomType::adbt_extend_inner;
		ClipperLib::PolyTree treeTop, treeBottom;
		double hTop, hBottom;

		_buildTop_onepoly(treeTop, hTop);
		_buildBottom_onepoly(treeBottom, hBottom);

		_buildFromSamePolyTree(&treeBottom, &treeTop);
	}

	void WidenInnerGenerator::buildShell()
	{
		m_adParam.bottom_type = ADBottomType::adbt_extend_inner;
		ClipperLib::PolyTree treeTop, treeBottom;
		double hTop, hBottom;

		_buildTop_onepoly(treeTop, hTop);
		_buildBottom_onepoly(treeBottom, hBottom);

		_buildFromSamePolyTree(&treeBottom, &treeTop);
	}

	void WidenInnerGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		m_adParam.bottom_type = ADBottomType::adbt_extend_inner;
		double hTop, hBottom;

		_buildTop_onepoly(topTree, hTop);
		_buildBottom_onepoly(bottomTree, hBottom);

// 		std::vector<double> area;
// 		areaPoly(topTree, area);
// 		_buildBoardPoly(&topTree);
// 		_buildBoardPoly(&bottomTree);
	}
}