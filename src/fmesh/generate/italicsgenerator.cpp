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

		//bottom
		ClipperLib::PolyTree bottomPloy;
		fmesh::offsetAndExtendPolyTree(m_poly, 0, thickness, 0, bottomPloy);
		std::vector<ClipperLib::PolyTree> botomSteppolys(3);
		if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
			//step
			if (btoomStepHeight <= bottomHeight)
			{
				fmesh::offsetAndExtendPolyTree(m_poly, 0, thickness, btoomStepHeight - 0.5, botomSteppolys.at(0));
				fmesh::offsetAndExtendPolyTree(m_poly, 0, thickness, btoomStepHeight, botomSteppolys.at(1));
				fmesh::offsetAndExtendPolyTree(m_poly, 0, thickness, btoomStepHeight, botomSteppolys.at(2));
				offsetExteriorInner(botomSteppolys.at(1), -btoomStepWiden);
				_buildFromDiffPolyTree(&botomSteppolys.at(0), &botomSteppolys.at(1));
				_buildFromDiffPolyTree(&botomSteppolys.at(1), &botomSteppolys.at(2));
			}
			_fillPolyTree(&bottomPloy, true);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_close)
		{
			fmesh::offsetAndExtendPolyTree(m_poly, 0, thickness, btoomStepHeight, botomSteppolys.at(0));
			_fillPolyTreeOutline(&bottomPloy, true);
			_fillPolyTreeOutline(&botomSteppolys.at(0));
		}
		else
		{
			fmesh::offsetAndExtendPolyTree(m_poly, 0, thickness, btoomStepHeight, botomSteppolys.at(0));
			_fillPolyTree(&bottomPloy, true);
		}
		//bottom end

		//middle
		std::vector<ClipperLib::PolyTree> polys(count + 1);
		for (int i = 0; i <= count; ++i)
		{
			fmesh::offsetAndExtendPolyTree(m_poly, -(float)i * offset, thickness, bottomHeight + (float)i * h, polys.at(i));
		}
		for (int i = 0; i < count; ++i)
		{
			ClipperLib::PolyTree out;
			_buildFromDiffPolyTree_drum(&polys.at(i), &polys.at(i + 1), 0, out);
			if (out.ChildCount() > 0)
			{
				_fillPolyTreeReverseInner(&out);
			}
		}

		//top
		if (m_adParam.top_type == ADTopType::adtt_step)
		{
			size_t num = count - h / topHeight;
			std::vector<ClipperLib::PolyTree> topSteppolys(3);

			fmesh::offsetAndExtendPolyTree(m_poly, -(float)num * offset, thickness, bottomHeight + (float)num * h - 0.5, topSteppolys.at(0));
			fmesh::offsetAndExtendPolyTree(m_poly, -(float)num * offset, thickness, bottomHeight + (float)num * h, topSteppolys.at(1));
			fmesh::offsetAndExtendPolyTree(m_poly, -(float)num * offset, thickness, bottomHeight + (float)num * h, topSteppolys.at(2));
			offsetExteriorInner(topSteppolys.at(1), -topStepWiden);
			_buildFromDiffPolyTree(&topSteppolys.at(0), &topSteppolys.at(1));
			_buildFromDiffPolyTree(&topSteppolys.at(1), &topSteppolys.at(2));

		}
		//top end 

		_buildFromSamePolyTree(&bottomPloy, &botomSteppolys.at(0));
		_buildFromSamePolyTree(&botomSteppolys.back(), &polys.front());
		_fillPolyTree(&polys.back());
	}
}