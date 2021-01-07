#include "reitalicsgenerator.h"

namespace fmesh
{
	ReItalicsGenerator::ReItalicsGenerator()
	{

	}

	ReItalicsGenerator::~ReItalicsGenerator()
	{

	}

	void ReItalicsGenerator::build()
	{
		//test data
// 		m_adParam.top_type = ADTopType::adtt_step;
// 		m_adParam.top_height = 1.0;
// 		m_adParam.shape_top_height = 2.0;
// 		m_adParam.top_extend_width = 0.5;
// 		m_adParam.bottom_type = ADBottomType::adbt_step;
// 		m_adParam.bottom_height = 1.0;
// 		m_adParam.bottom_extend_width = 0.5;
// 		m_adParam.shape_bottom_height = 3.0;
		//

		//init
		float btoomStepHeight = m_adParam.shape_bottom_height;
		float shape_top_height = m_adParam.shape_top_height;
		float thickness = m_adParam.extend_width / 2.0;
		float bottomHeight = m_adParam.bottom_height;
		float topStepWiden = m_adParam.top_extend_width / 2.0;
		float btoomStepWiden = m_adParam.bottom_extend_width / 2.0;					

		//modify
		if (m_adParam.top_type == ADTopType::adtt_step)
		{
			shape_top_height = m_adParam.shape_top_height > m_adParam.top_height ? m_adParam.shape_top_height : m_adParam.top_height;
		}

		float middleHeight = m_adParam.total_height - shape_top_height;
		size_t count = (middleHeight - bottomHeight) / 0.5;
		float offset = 0.5 * tan(m_adParam.shape_angle) / 4;

		std::vector<ClipperLib::PolyTree> Steppolys(1);
		size_t num =count - bottomHeight / 0.5;

		//bottom
		std::vector<ClipperLib::PolyTree> middlePolys(count + 1);
		for (int i = 0; i <= count; ++i)
		{	
			float _offset = offset * i;
			float delta = middleHeight - (float)i * 0.5;
			fmesh::offsetAndExtendPolyTree(m_poly, offset*i/2, thickness, delta, middlePolys.at(i)); 
			if (!middlePolys.at(i).ChildCount())
			{
				count = i;
			}
		}
		for (int i = 0; i < count; ++i)
		{
// 			ClipperLib::PolyTree out;
// 			_buildFromDiffPolyTree_drum(&middlePolys.at(i), &middlePolys.at(i + 1), 0, out);
// 
// 			if (out.ChildCount() > 0)
// 			{
// 				_fillPolyTreeReverseInner(&out);
// 			}
			//_buildFromDiffPolyTree_diff(&middlePolys.at(i), &middlePolys.at(i + 1));
			_buildFromDiffPolyTree_diffSafty(&middlePolys.at(i), &middlePolys.at(i + 1));
		}

		if (m_adParam.top_type != ADTopType::adtt_none)
			_buildTopBottom(&middlePolys.back(), &middlePolys.front(), offset * count / 2);
		else
			_buildTopBottom(nullptr, &middlePolys.front());
		
	}
}