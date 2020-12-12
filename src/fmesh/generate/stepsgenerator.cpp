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
		double thickness = m_adParam.extend_width / 2.0f;
		double topHeight = m_adParam.top_height;
		double bottomHeight = m_adParam.total_height - topHeight;

		std::vector<ClipperLib::PolyTree> polyTrees(5);
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, 0, polyTrees.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, bottomHeight- thickness, polyTrees.at(1));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, bottomHeight, polyTrees.at(2));
		offsetAndExtendPolyTree(m_poly, -thickness, thickness, bottomHeight, polyTrees.at(3));
		offsetAndExtendPolyTree(m_poly, -thickness, thickness, m_adParam.total_height, polyTrees.at(4));

		_buildFromSamePolyTree(&polyTrees.at(0), &polyTrees.at(1));
		_buildFromDiffPolyTree(&polyTrees.at(2), &polyTrees.at(3));
		_buildFromSamePolyTree(&polyTrees.at(3), &polyTrees.at(4));
		offsetExteriorInner(polyTrees.at(2), -thickness);
		_buildFromDiffPolyTree(&polyTrees.at(1), &polyTrees.at(2));

		_fillPolyTree(&polyTrees.front(), true);
		_fillPolyTree(&polyTrees.back());
	}
}