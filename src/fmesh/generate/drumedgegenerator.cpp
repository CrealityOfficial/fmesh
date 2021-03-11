#include "drumedgegenerator.h"

namespace fmesh
{
	DrumedgeGenerator::DrumedgeGenerator()
	{

	}

	DrumedgeGenerator::~DrumedgeGenerator()
	{

	}

	void DrumedgeGenerator::build()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);
		_buildTopBottom(&middlePolys.front(), &middlePolys.back());
	}

	void DrumedgeGenerator::buildShell()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys,true);
		_buildTopBottom_onepoly(&middlePolys.front(), &middlePolys.back());
	}

	void DrumedgeGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		copy2PolyTree(middlePolys.back(), topTree);
		copy2PolyTree(middlePolys.front(), bottomTree);
	}

	void DrumedgeGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePloy)
	{
		//initTestData()

		float shape_bottom_height = m_adParam.shape_bottom_height;
		float shape_top_height = m_adParam.shape_top_height;
		float shape_middle_width = m_adParam.shape_middle_width;
		float thickness = m_adParam.extend_width / 2.0;

		//modify
		if (m_adParam.top_type == ADTopType::adtt_step)
		{
			shape_top_height = m_adParam.shape_top_height > m_adParam.top_height ? m_adParam.shape_top_height : m_adParam.top_height;
		}
		if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
			shape_bottom_height = m_adParam.shape_bottom_height > m_adParam.bottom_height ? m_adParam.shape_bottom_height : m_adParam.bottom_height;
		}

		float middleHeight = m_adParam.total_height - shape_bottom_height - shape_top_height;

		size_t drumHCount = 32;
		double drumDelta = middleHeight / (double)drumHCount;
		middlePolys.resize(1 + drumHCount);

		float offset = 3.1415926 / drumHCount;

		size_t middle = drumHCount / 2;
		std::vector<float> offs(drumHCount + 1);
		for (size_t i = 0; i < drumHCount + 1; i++)
		{
			float delta = shape_bottom_height + i * drumDelta;
			float _offset = shape_middle_width * sin((offset * i) > 0 ? offset * i : 0);
			if (onePloy)
			{
				offsetPolyTree(m_poly, _offset / 2, middlePolys.at(i));
				setPolyTreeZ(middlePolys.at(i), delta);
			} 
			else
			{
				offsetAndExtendPolyTree(m_poly, _offset / 2, thickness, delta, middlePolys.at(i));
				_simplifyPoly(&middlePolys.at(i));
			}
			offs.at(i) = _offset;
		}

		for (size_t i = 0; i < drumHCount; i++)
		{
			if (onePloy)
			{
				_buildFromDiffPolyTree_onePoly(&middlePolys.at(i), &middlePolys.at(i + 1));
			}
			else
			{
				_buildFromDiffPolyTree_diffSafty(&middlePolys.at(i), &middlePolys.at(i + 1));
			}
		}
	}

	void DrumedgeGenerator::initTestData()
	{
		m_adParam.top_type = ADTopType::adtt_step;
		m_adParam.top_height = 1.0;
		m_adParam.shape_top_height = 2.0;
		m_adParam.bottom_type = ADBottomType::adbt_step;
		m_adParam.bottom_height = 1.0;
		m_adParam.shape_bottom_height = 3.0;
		m_adParam.shape_middle_width = 6.0;
	}

}