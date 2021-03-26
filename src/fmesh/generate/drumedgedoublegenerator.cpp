#include "drumedgedoublegenerator.h"

namespace fmesh
{
	DrumedgeDoubleGenerator::DrumedgeDoubleGenerator()
	{

	}

	DrumedgeDoubleGenerator::~DrumedgeDoubleGenerator()
	{

	}

	void DrumedgeDoubleGenerator::build()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);
		_buildTopBottom(&middlePolys.front(), &middlePolys.back());
	}

	void DrumedgeDoubleGenerator::buildShell()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		_buildTopBottom_onepoly(&middlePolys.front(), &middlePolys.back());
	}

	void DrumedgeDoubleGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		copy2PolyTree(middlePolys.back(), topTree);
		copy2PolyTree(middlePolys.front(), bottomTree);
	}

	void DrumedgeDoubleGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePloy)
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

		float middleHeightT = middleHeight / 2.0f;
		float middleHeightB = middleHeight / 2.0f;
		size_t drumHCount = 32;
		size_t drumHCount1 = drumHCount / 2;
		middlePolys.resize(drumHCount);

		{
			//bottom		
			double drumDelta = middleHeightT / (double)drumHCount1;
			float offset = 3.1415926 / drumHCount1;
			size_t middle = drumHCount1 / 2;
			for (size_t i = 0; i < drumHCount1; i++)
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
			}
		}

		{
			//top
			size_t drumHCount2 = drumHCount- drumHCount1;
			double drumDelta = middleHeightB / (double)drumHCount2;
			float offset = 3.1415926 / drumHCount2;
			for (size_t i = 0; i < drumHCount2; i++)
			{
				float delta = shape_bottom_height + (drumHCount2+i) * drumDelta;
				float _offset = shape_middle_width * sin((offset * i) > 0 ? offset * i : 0);
				if (onePloy)
				{
					offsetPolyTree(m_poly, _offset / 2, middlePolys.at(drumHCount1 + i));
					setPolyTreeZ(middlePolys.at(drumHCount1 + i), delta);
				}
				else
				{
					offsetAndExtendPolyTree(m_poly, _offset / 2, thickness, delta, middlePolys.at(drumHCount1+ i));
					_simplifyPoly(&middlePolys.at(drumHCount1+ i));
				}
			}
		}

		for (size_t i = 0; i < drumHCount-1; i++)
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

	void DrumedgeDoubleGenerator::initTestData()
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