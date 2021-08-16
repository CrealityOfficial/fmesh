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

		_buildTop(treeTop, hTop);
		_buildBottom(treeBottom, hBottom);

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

		ClipperLib::PolyTree _treeTop, _treeBottom;

		_buildTop_onepoly(_treeTop, hTop);
		_buildBottom_onepoly(_treeBottom, hBottom);

		offsetPolyType(_treeTop, m_adParam.exoprtParam.bottom_offset, topTree, m_adParam.bluntSharpCorners);
		offsetPolyType(_treeBottom, m_adParam.exoprtParam.top_offset, bottomTree, m_adParam.bluntSharpCorners);
	}
}