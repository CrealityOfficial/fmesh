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
			_buildTopBottom(&middlePolys.front(), &middlePolys.back(), 0, middleoffset);
		else
		{
			_buildTopBottom(&middlePolys.front(), nullptr);
			_fillPolyTree(&middlePolys.back());
		}
	}

	void ItalicsGenerator::buildShell()
	{
		double middleoffset = 0;
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset);

		m_adParam.bottom_type = ADBottomType::adbt_none;
		m_adParam.top_type = ADTopType::adtt_none;

		if (m_adParam.top_type != ADTopType::adtt_none)
			_buildTopBottom(&middlePolys.front(), &middlePolys.back(), 0, middleoffset);
		else
		{
			_buildTopBottom(&middlePolys.front(), nullptr);
			//_fillPolyTree(&middlePolys.back());
		}
	}

	void ItalicsGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		double middleoffset = 0;
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset);

		topTree = middlePolys.back();
		bottomTree = middlePolys.front();
	}

	void ItalicsGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, double& middleoffset)
	{
		//initTestData()

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

		float middleHeight = m_adParam.total_height - bottomHeight - topHeight;
		int count = middleHeight / 0.5;;
		float offset = middleHeight * std::sin(10 / 2 * 3.1415926 / 180.0) / (float)count;
		float h = middleHeight / (float)count;

		std::vector<ClipperLib::PolyTree> Steppolys(1);
		size_t num = (h > 0) ? (count - topHeight / h) : 0;
		//middle
		middlePolys.resize(count + 1);
		for (int i = 0; i <= count; ++i)
		{
			fmesh::offsetAndExtendPolyTree(m_poly, -(float)i * offset, thickness, bottomHeight + (float)i * h, middlePolys.at(i));
		}
		for (int i = 0; i < count; ++i)
		{
			//if (i != 40)
			//	continue;
			//_buildFromDiffPolyTree_diffSafty(&middlePolys.at(i), &middlePolys.at(i + 1));
			_buildFromDiffPolyTree_all(&middlePolys.at(i), &middlePolys.at(i + 1), thickness / 2.0f);
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
	}

}