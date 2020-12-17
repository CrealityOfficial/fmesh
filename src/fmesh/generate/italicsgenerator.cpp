#include "italicsgenerator.h"

namespace fmesh
{
	ItalicsGenerator::ItalicsGenerator()
	{

	}

	ItalicsGenerator::~ItalicsGenerator()
	{

	}

	void ItalicsGenerator::build()
	{
		//test data
// 		m_adParam.top_type = ADTopType::adtt_step;
// 		m_adParam.top_height = 5.0;
// 		m_adParam.shape_top_height = 2.0;
// 		m_adParam.bottom_type = ADBottomType::adbt_close;
// 		m_adParam.bottom_height = 1.0;
// 		m_adParam.shape_bottom_height = 3.0;
		//

		//init
		float btoomStepHeight = m_adParam.shape_bottom_height;
		float bottomHeight = m_adParam.bottom_height;
		float thickness = m_adParam.extend_width / 2.0;
		float topHeight = m_adParam.top_height;
		float topStepWiden = m_adParam.top_extend_width / 2.0;
		float btoomStepWiden = m_adParam.bottom_extend_width / 2.0;

		//modify
		if (m_adParam.bottom_type == ADBottomType::adbt_step
			|| m_adParam.bottom_type == ADBottomType::adbt_close)
		{
			bottomHeight = m_adParam.bottom_height < m_adParam.shape_bottom_height ? m_adParam.shape_bottom_height : m_adParam.bottom_height;
		}

		float middleHeight = m_adParam.total_height - bottomHeight;
		int count = 10;
		float offset = middleHeight * std::sin(10 / 2 * 3.1415926 / 180.0) / (float)count;
		float h = middleHeight / (float)count;

		//middle
		std::vector<ClipperLib::PolyTree> middlePolys(count + 1);
		for (int i = 0; i <= count; ++i)
		{
			fmesh::offsetAndExtendPolyTree(m_poly, -(float)i * offset, thickness, bottomHeight+(float)i * h, middlePolys.at(i));
		}
		for (int i = 0; i < count; ++i)
		{
			ClipperLib::PolyTree out;
			_buildFromDiffPolyTree_drum(&middlePolys.at(i), &middlePolys.at(i + 1), 0, out);
			if (out.ChildCount() > 0)
			{
				_fillPolyTreeReverseInner(&out);
			}
		}

		//top
		if (m_adParam.top_type == ADTopType::adtt_step)
		{
			if (topHeight)
			{
				size_t num = count - topHeight / h;
				std::vector<ClipperLib::PolyTree> topSteppolys(3);

				fmesh::offsetAndExtendPolyTree(m_poly, -(float)num * offset, thickness, bottomHeight + (float)num * h - 0.5, topSteppolys.at(0));
				fmesh::offsetAndExtendPolyTree(m_poly, -(float)num * offset, thickness, bottomHeight + (float)num * h, topSteppolys.at(1));
				fmesh::offsetAndExtendPolyTree(m_poly, -(float)num * offset, thickness, bottomHeight + (float)num * h, topSteppolys.at(2));
				offsetExteriorInner(topSteppolys.at(1), topStepWiden);
				_buildFromDiffPolyTree(&topSteppolys.at(0), &topSteppolys.at(1));
				_buildFromDiffPolyTree(&topSteppolys.at(1), &topSteppolys.at(2));
			}
		}
		//top end 

		_fillPolyTree(&middlePolys.back());

		ClipperLib::PolyTree treeBottom;
		double hBottom;
		_buildBottom(treeBottom, hBottom);
		_buildFromSamePolyTree(&treeBottom, &middlePolys.front());
	}
}