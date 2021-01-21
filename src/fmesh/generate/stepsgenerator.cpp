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
		//test data
// 		m_adParam.top_type = ADTopType::adtt_step;
// 		m_adParam.top_height = 1.0;
// 		m_adParam.shape_top_height = 2.0;
// 		m_adParam.bottom_type = ADBottomType::adbt_extend_outter;
// 		m_adParam.bottom_height = 1.0;
// 		m_adParam.shape_bottom_height = 3.0;
		//

		double thickness = m_adParam.extend_width / 2.0f;
		double topHeight = m_adParam.shape_top_height;
		double bottomHeight = m_adParam.total_height - topHeight;

		std::vector<ClipperLib::PolyTree> middlePolys(4);
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, bottomHeight- thickness, middlePolys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, bottomHeight, middlePolys.at(1));
		offsetAndExtendPolyTree(m_poly, -thickness, thickness, bottomHeight, middlePolys.at(2));
		offsetAndExtendPolyTree(m_poly, -thickness, thickness, m_adParam.total_height, middlePolys.at(3));

		double dealt = 1.0;
		_buildFromDiffPolyTree_xor(&middlePolys.at(1), &middlePolys.at(2), dealt,1);
		_buildFromSamePolyTree(&middlePolys.at(2), &middlePolys.at(3));
		offsetExteriorInner(middlePolys.at(1), thickness*2);
		_buildFromDiffPolyTree_diffSafty(&middlePolys.at(0), &middlePolys.at(1));
		_buildFromDiffPolyTree_xor(&middlePolys.at(1), &middlePolys.at(2), dealt,2);
		_fillPolyTree(&middlePolys.back());

		_buildTopBottom(&middlePolys.front(), nullptr);
	}
}