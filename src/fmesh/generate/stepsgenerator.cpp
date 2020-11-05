#include "stepsgenerator.h"

namespace fmesh
{
	StepsGenerator::StepsGenerator()
	{

	}

	StepsGenerator::~StepsGenerator()
	{

	}

	void StepsGenerator::build()
	{
		double thickness = m_param.thickness / 2.0f;
		double offset = -thickness;
		std::vector<float> heights(4);
		heights.at(0) = 0.0f;
		heights.at(1) = m_param.totalH - m_param.topH - m_param.connectDepth;
		heights.at(2) = m_param.totalH - m_param.topH;
		heights.at(3) = m_param.totalH;
		std::vector<ClipperLib::PolyTree> polyTrees(4);

		extendPolyTree(m_poly, thickness, polyTrees.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(1), polyTrees.at(1));
		offsetAndExtendPolyTree(m_poly, offset, thickness, heights.at(2), polyTrees.at(2));
		offsetAndExtendPolyTree(m_poly, offset, thickness, heights.at(3), polyTrees.at(3));

		_buildFromSamePolyTree(&polyTrees.at(0), &polyTrees.at(1));
		_buildFromDiffPolyTree(&polyTrees.at(1), &polyTrees.at(2));
		_buildFromSamePolyTree(&polyTrees.at(2), &polyTrees.at(3));
		_fillPolyTree(&polyTrees.at(0), true);
		_fillPolyTree(&polyTrees.at(3));
	}
}