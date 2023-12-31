#include "simplegenerator.h"
#include "fmesh/generate/triangularization.h"

namespace fmesh
{
	SimpleGenerator::SimpleGenerator()
	{

	}

	SimpleGenerator::~SimpleGenerator()
	{

	}

	void SimpleGenerator::build()
	{
		ClipperLibXYZ::PolyTree treeTop, treeBottom;
		double hTop, hBottom;

		_buildTop(treeTop, hTop);
		if (m_tracer)
		{
			m_tracer->progress(0.2f);
			if (m_tracer->interrupt())
				return;
		}

		_buildBottom(treeBottom, hBottom);

		if (m_tracer)
		{
			m_tracer->progress(0.4f);
			if (m_tracer->interrupt())
				return;
		}

		_buildFromSamePolyTree(&treeBottom, &treeTop);

		if (m_tracer)
			m_tracer->progress(0.8f);
	}

	void SimpleGenerator::buildShell()
	{
		ClipperLibXYZ::PolyTree treeTop, treeBottom;
		double hTop, hBottom;

		_buildTop_onepoly(treeTop, hTop,-m_adParam.extend_width/2.0);
		_buildBottom_onepoly(treeBottom, hBottom, -m_adParam.extend_width / 2.0);

		_buildFromSamePolyTree(&treeBottom, &treeTop);
	}

	void SimpleGenerator::buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree)
	{
		double hTop, hBottom;
		ClipperLibXYZ::PolyTree _treeTop, _treeBottom;

		_buildTop_onepoly(_treeTop, hTop);
		_buildBottom_onepoly(_treeBottom, hBottom);

		offsetPolyType(_treeTop, m_adParam.exoprtParam.bottom_offset_other, topTree, m_adParam.bluntSharpCorners);
		offsetPolyType(_treeBottom, m_adParam.exoprtParam.top_offset_other, bottomTree, m_adParam.bluntSharpCorners);

// 		std::vector<double> area;
// 		areaPoly(topTree, area);
// 		_buildBoardPoly(&topTree);
// 		_buildBoardPoly(&bottomTree);
	}
}