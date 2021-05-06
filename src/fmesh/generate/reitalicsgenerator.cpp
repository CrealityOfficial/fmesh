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
		std::vector<ClipperLib::PolyTree> middlePolys;
		double middleoffset = 0;

		buildMiddle(middlePolys, middleoffset);

		if (m_adParam.top_type != ADTopType::adtt_none)
			_buildTopBottomDiff(&middlePolys.front(), &middlePolys.back(), 0, middleoffset);
		else
		{
			_buildTopBottomDiff(&middlePolys.front(), nullptr);
			_fillPolyTree(&middlePolys.back());
		}
	}

	void ReItalicsGenerator::buildShell()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		double middleoffset = 0;
		buildMiddle(middlePolys, middleoffset,true);
		//_buildTopBottom_onepoly(&middlePolys.front(), &middlePolys.back(), 0, middleoffset);
	}

	void ReItalicsGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		double middleoffset = 0;
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset, true);
		copy2PolyTree(middlePolys.back(), topTree);
		copy2PolyTree(middlePolys.front(), bottomTree);
	}

	void ReItalicsGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, double& middleoffset,bool onePoly)
	{
		//initTestData();

		//init
		float btoomStepHeight = m_adParam.shape_bottom_height;
		float shape_top_height = m_adParam.shape_top_height;
		float thickness = m_adParam.extend_width / 2.0;
		float bottomHeight = m_adParam.bottom_height;
		float topHeight = m_adParam.top_height;
		float topStepWiden = m_adParam.top_extend_width / 2.0;
		float btoomStepWiden = m_adParam.bottom_extend_width / 2.0;

		//modify
		if (m_adParam.top_type == ADTopType::adtt_step)
		{
			shape_top_height = m_adParam.shape_top_height > m_adParam.top_height ? m_adParam.shape_top_height : m_adParam.top_height;
		}

		float middleHeight = m_adParam.total_height - bottomHeight - topHeight;
		int count = middleHeight / 0.5;;
		float offset = middleHeight * std::sin(m_adParam.shape_angle / 2 * 3.1415926 / 180.0) / (float)count;
		float h = middleHeight / (float)count;

		std::vector<ClipperLib::PolyTree> Steppolys(1);
		size_t num = (h > 0) ? (count - topHeight / h) : 0;
		//middle
		middlePolys.resize(count + 1);
		for (int i = 0; i <= count; ++i)
		{
			if (onePoly)
			{
				offsetPolyTree(m_poly, (float)i * offset, middlePolys.at(i));
				passivationPolyTree(middlePolys.at(i), thickness / 2.0);
				setPolyTreeZ(middlePolys.at(i), bottomHeight + (float)i * h);
			} 
			else
			{	
				//offsetAndExtendPolyTreeMiter(m_poly, (float)i * offset, thickness, middlePolys.at(i));
				//setPolyTreeZ(middlePolys.at(i),bottomHeight + (float)i * h);
				offsetAndExtendPolyTree(m_poly, (float)i * offset, thickness, bottomHeight + (float)i * h, middlePolys.at(i));
				//if (i)
				//{
				//	if ((middlePolys.at(i).Total() + middlePolys.at(i - 1).Total())%2)
				//	{
				//		middlePolys.at(i).Clear();
				//	}
				//}
			}
		}

		for (int i =0; i < middlePolys.size()-1; ++i)
		{
			//int j = i + 1;
			//while (!middlePolys.at(j).Total())
			//{
			//	++j;
			//	if (j >= middlePolys.size())
			//		break;
			//}
			if (onePoly)
				_buildFromDiffPolyTree_onePoly(&middlePolys.at(i), &middlePolys.at(i + 1));//outer
			else
			{
				//_buildFromDiffPolyTree_all(&middlePolys.at(i), &middlePolys.at(j), thickness / 2.0, 0, false, true);
				_buildFromDiffPolyTree_diff(&middlePolys.at(i), &middlePolys.at(i + 1));
			}
			//i += j - i - 1;
		}
		middleoffset = offset * count;
	}

	void ReItalicsGenerator::initTestData()
	{
		m_adParam.top_type = ADTopType::adtt_step;
		m_adParam.top_height = 1.0;
		m_adParam.shape_top_height = 2.0;
		m_adParam.top_extend_width = 0.5;
		m_adParam.bottom_type = ADBottomType::adbt_step;
		m_adParam.bottom_height = 2.0;
		m_adParam.bottom_extend_width = 0.5;
		m_adParam.shape_bottom_height = 3.0;
		m_adParam.total_height = 15;
	}
}