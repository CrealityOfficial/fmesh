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
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);

		_buildTopBottom(&middlePolys.front(), nullptr);
	}

	void StepsGenerator::buildShell()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		_buildTopBottom_onepoly(&middlePolys.front(), nullptr);
	}

	void StepsGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		copy2PolyTree(middlePolys.back(), topTree);
		copy2PolyTree(middlePolys.front(), bottomTree);
	}

	void StepsGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePoly)
	{
		double thickness = m_adParam.extend_width / 2.0f;
		double topHeight = m_adParam.shape_top_height;
		double bottomHeight = m_adParam.total_height - topHeight;

		middlePolys.resize(4);
		if (onePoly)
		{
			copy2PolyTree(m_poly, middlePolys.at(0));
			setPolyTreeZ(middlePolys.at(0), bottomHeight);
			offsetPolyTree(m_poly, -thickness, middlePolys.at(1));
			setPolyTreeZ(middlePolys.at(1), bottomHeight);
			offsetPolyTree(m_poly, -thickness, middlePolys.at(2));
			setPolyTreeZ(middlePolys.at(2), m_adParam.total_height);
		} 
		else
		{
			offsetAndExtendPolyTree(m_poly, 0.0, thickness, bottomHeight - thickness, middlePolys.at(0));
			offsetAndExtendPolyTree(m_poly, 0.0, thickness, bottomHeight, middlePolys.at(1));
			offsetAndExtendPolyTree(m_poly, -thickness, thickness, bottomHeight, middlePolys.at(2));
			offsetAndExtendPolyTree(m_poly, -thickness, thickness, m_adParam.total_height, middlePolys.at(3));
		}

		double dealt = 1.0;
		if (onePoly)
		{
			_buildFromDiffPolyTree_xor(&middlePolys.at(0), &middlePolys.at(1));
			_buildFromSamePolyTree(&middlePolys.at(1), &middlePolys.at(2));
		} 
		else
		{
			_buildFromDiffPolyTree_xor(&middlePolys.at(1), &middlePolys.at(2), dealt, 3);
			_buildFromSamePolyTree(&middlePolys.at(2), &middlePolys.at(3));
			offsetExteriorInner(middlePolys.at(1), thickness);
			_buildFromDiffPolyTree_diffSafty(&middlePolys.at(0), &middlePolys.at(1));
			_buildFromDiffPolyTree_xor(&middlePolys.at(1), &middlePolys.at(2), dealt, 2);
			_fillPolyTree(&middlePolys.back());
		}
	}

	void StepsGenerator::initTestData()
	{
		m_adParam.top_type = ADTopType::adtt_step;
		m_adParam.top_height = 1.0;
		m_adParam.shape_top_height = 2.0;
		m_adParam.bottom_type = ADBottomType::adbt_extend_outter;
		m_adParam.bottom_height = 1.0;
		m_adParam.shape_bottom_height = 3.0;
	}

}