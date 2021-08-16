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
		//initTestData();

		double middleoffset = 0;
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset);

		if (m_adParam.top_type != ADTopType::adtt_none)
			_buildTopBottomDiff(&middlePolys.front(), &middlePolys.back(), 0, middleoffset);
		else
		{
			_buildTopBottomDiff(&middlePolys.front(), nullptr,0, middleoffset);
			_fillPolyTree(&middlePolys.back());
		}
	}

	void ItalicsGenerator::buildShell()
	{
		double middleoffset = 0;
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset,true);
		_buildTopBottom_onepoly(&middlePolys.front(), &middlePolys.back(), 0, middleoffset);
	}

	void ItalicsGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		double middleoffset = 0;
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset, true);

		offsetPolyType(middlePolys.back(), m_adParam.exoprtParam.top_offset, topTree, m_adParam.bluntSharpCorners);
		offsetPolyType(middlePolys.front(), m_adParam.exoprtParam.bottom_offset, bottomTree, m_adParam.bluntSharpCorners);
	}

	void ItalicsGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, double& middleoffset, bool onePoly)
	{
		//initTestData();

		//init
		float btoomStepHeight = m_adParam.shape_bottom_height;
		float bottomHeight=m_adParam.bottom_height;
		float thickness = m_adParam.extend_width / 2.0;
		float topHeight = m_adParam.top_height;
		float topStepWiden = m_adParam.top_extend_width / 2.0;
		float btoomStepWiden = m_adParam.bottom_extend_width / 2.0;

		//modify
		if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
			bottomHeight = m_adParam.bottom_height < m_adParam.shape_bottom_height ? m_adParam.shape_bottom_height : m_adParam.bottom_height;
		}
		else if(m_adParam.bottom_type == ADBottomType::adbt_close)
		{
			bottomHeight = m_adParam.bottom_layers > 0 ? m_adParam.bottom_extend_width * m_adParam.bottom_layers : m_adParam.bottom_extend_width;
		}
		else
		{
			bottomHeight = 0;
		}

		if (m_adParam.top_type != ADTopType::adtt_step)
		{
			topHeight = 0;
		}

		float middleHeight = m_adParam.total_height - bottomHeight - topHeight;
		int count = middleHeight / 0.5;;
		//float offset = middleHeight * std::sin(m_adParam.shape_angle / 2 * 3.1415926 / 180.0) / (float)count;
		float offset = m_adParam.shape_angle / 2.0 / (float)count;
		float h = middleHeight / (float)count;

		std::vector<ClipperLib::PolyTree> Steppolys(1);
		size_t num = (h > 0) ? (count - topHeight / h) : 0;
		//middle
		middlePolys.resize(count + 1);
		for (int i = 0; i <= count; ++i)
		{
			if (onePoly)
			{
				offsetPolyType(m_poly, -(float)i * offset, middlePolys.at(i),m_adParam.bluntSharpCorners);
				passivationPolyTree(middlePolys.at(i), thickness / 2.0);
				setPolyTreeZ(middlePolys.at(i), bottomHeight + (float)i * h);	
			} 
			else
			{
				offsetAndExtendpolyType(m_poly, -(float)i * offset, thickness, bottomHeight + (float)i * h, middlePolys.at(i),m_adParam.bluntSharpCorners);
			}
		}

		for (int i = 0; i < count; ++i)
		{
			if (onePoly)
				_buildFromDiffPolyTree_onePoly(&middlePolys.at(i), &middlePolys.at(i + 1));//outer
			else
			{
				_buildFromDiffPolyTree_diff(&middlePolys.at(i), &middlePolys.at(i + 1));
			}
		}
		middleoffset = -(float)count * offset;
	}

	void ItalicsGenerator::initTestData()
	{
		m_adParam.top_type = ADTopType::adtt_step;
		m_adParam.top_height = 5.0;
		m_adParam.shape_top_height = 2.0;
		m_adParam.top_extend_width = 0.5;
		m_adParam.bottom_type = ADBottomType::adbt_step;
		m_adParam.bottom_height = 1.0;
		m_adParam.bottom_extend_width = 0.5;
		m_adParam.shape_bottom_height = 3.0;
		m_adParam.total_height = 15;
	}

}