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
		//double bottomHeight=


		//double thickness = m_param.thickness / 2.0f;
		double offset = -thickness;
		std::vector<float> heights(4);
		heights.at(0) = 0.0f;
		heights.at(1) = m_param.totalH - m_param.topH - m_param.connectDepth;
		heights.at(2) = m_param.totalH - m_param.topH;
		heights.at(3) = m_param.totalH;
		std::vector<ClipperLib::PolyTree> polyTrees(4);

		//extendPolyTree(m_poly, thickness, polyTrees.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(0), polyTrees.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(1), polyTrees.at(1));
		offsetAndExtendPolyTree(m_poly, offset, thickness, heights.at(2), polyTrees.at(2));
		offsetAndExtendPolyTree(m_poly, offset, thickness, heights.at(3), polyTrees.at(3));

		_buildFromSamePolyTree(&polyTrees.at(0), &polyTrees.at(1));
		_buildFromDiffPolyTree(&polyTrees.at(1), &polyTrees.at(2));
		_buildFromSamePolyTree(&polyTrees.at(2), &polyTrees.at(3));

		_fillPolyTree(&polyTrees.at(0), true);
		_fillPolyTree(&polyTrees.at(3));

// 		double thickness = m_param.thickness / 2.0f;
// 		double offset = -thickness;
// 		std::vector<float> heights(4);
// 		heights.at(0) = 0.0f;
// 		heights.at(1) = m_param.totalH - m_param.topH - m_param.connectDepth;
// 		heights.at(2) = m_param.totalH - m_param.topH;
// 		heights.at(3) = m_param.totalH;
// 		std::vector<ClipperLib::PolyTree> polyTrees(4);
// 
// 		if (EBottomMode::STEP == m_param.bottomMode)
// 		{
// 			std::vector<ClipperLib::PolyTree> polyTrees(2);
// 			offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_param.bottomStepW, polyTrees[0]);
// 			offsetAndExtendPolyTree(m_poly, -m_param.bottomStepW, thickness, m_param.bottomStepW + thickness, polyTrees[1]);
// 			_buildFromDiffPolyTree(&polyTrees.at(0), &polyTrees.at(1));
// 			_fillPolyTree(&polyTrees.at(1));
// 		}
// 		else if (EBottomMode::WIDEN == m_param.bottomMode)
// 		{
// 			float bottomWidenH = 0.5f;
// 			heights.at(0) = 0.0f+ bottomWidenH;
// 			heights.at(1) = m_param.totalH - m_param.topH - m_param.connectDepth + bottomWidenH;;
// 			heights.at(2) = m_param.totalH - m_param.topH + bottomWidenH;;
// 			heights.at(3) = m_param.totalH + bottomWidenH;;
// 
// 			std::vector<ClipperLib::PolyTree> polyTrees_widen(4);
// 			offsetAndExtendPolyTree(m_poly, m_param.bottomWidenW, thickness, 0.0f, polyTrees_widen[0]);
// 			offsetAndExtendPolyTree(m_poly, m_param.bottomWidenW, thickness, 0.5f, polyTrees_widen[1]);
// 			offsetAndExtendPolyTree(m_poly, 0, thickness, 0.0f, polyTrees_widen[2]);
// 			offsetAndExtendPolyTree(m_poly, 0, thickness, 0.5f, polyTrees_widen[3]);
// 
// 			//_buildFromDiffPolyTree(&polyTrees_widen.at(0), &polyTrees_widen.at(1));
// 			//_buildFromDiffPolyTree(&polyTrees_widen.at(1), &polyTrees_widen.at(3));
// 
// 			//_buildFromDiffPolyTree(&polyTrees_widen.at(0), &polyTrees_widen.at(2));
// 			//_buildFromDiffPolyTree(&polyTrees_widen.at(0), &polyTrees.at(0));
// 			//_buildFromDiffPolyTree(&polyTrees_widen.at(1), &polyTrees_widen.at(2));
// 
// 			//_fillPolyTree(&polyTrees_widen.at(0));
// 			_fillPolyTreeOutline(&polyTrees_widen.at(0));
// 			//_fillPolyTree(&polyTrees_widen.at(1), true);
// 			//_fillPolyTreeOutline(&polyTrees_widen.at(1));
// 		}
// 
// 		//extendPolyTree(m_poly, thickness, polyTrees.at(0));
// 		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(0), polyTrees.at(0));
// 		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(1), polyTrees.at(1));
// 		offsetAndExtendPolyTree(m_poly, offset, thickness, heights.at(2), polyTrees.at(2));
// 		offsetAndExtendPolyTree(m_poly, offset, thickness, heights.at(3), polyTrees.at(3));
// 
// 		_buildFromSamePolyTree(&polyTrees.at(0), &polyTrees.at(1));
// 		_buildFromDiffPolyTree(&polyTrees.at(1), &polyTrees.at(2));
// 		_buildFromSamePolyTree(&polyTrees.at(2), &polyTrees.at(3));
// 
// 		_fillPolyTree(&polyTrees.at(0), true);
// 		_fillPolyTree(&polyTrees.at(3));
	}
}