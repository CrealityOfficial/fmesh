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
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys);
		if (middlePolys.size())
			_buildTopBottomDiff(&middlePolys.front(), &middlePolys.back());
	}

	void DrumedgeDoubleGenerator::buildShell()
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		if (middlePolys.size())
			_buildTopBottom_onepoly(&middlePolys.front(), &middlePolys.back());
	}

	void DrumedgeDoubleGenerator::buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree)
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);

		if (middlePolys.size())
		{
			offsetPolyType(middlePolys.back(), m_adParam.exoprtParam.top_offset, topTree, m_adParam.bluntSharpCorners);
			offsetPolyType(middlePolys.front(), m_adParam.exoprtParam.bottom_offset, bottomTree, m_adParam.bluntSharpCorners);
		}
	}

	void DrumedgeDoubleGenerator::buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys, bool onePloy)
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
		else if (m_adParam.top_type == ADTopType::adtt_close)
		{
			shape_top_height = m_adParam.top_layers > 0 ? m_adParam.top_extend_width * m_adParam.top_layers : m_adParam.top_extend_width;
		}
		else
		{
			shape_top_height = 0;
		}

		if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
			shape_bottom_height = m_adParam.shape_bottom_height > m_adParam.bottom_height ? m_adParam.shape_bottom_height : m_adParam.bottom_height;
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_close)
		{
			shape_bottom_height = m_adParam.bottom_layers > 0 ? m_adParam.bottom_extend_width * m_adParam.bottom_layers : m_adParam.bottom_extend_width;
		}
		else
		{
			shape_bottom_height = 0;
		}

		float middleHeight = m_adParam.total_height - shape_bottom_height - shape_top_height;

		float middleHeightT = middleHeight / 2.0f;
		float middleHeightB = middleHeight / 2.0f;
		size_t drumHCount = 32;
		size_t drumHCount1 = drumHCount / 2;
		middlePolys.resize(drumHCount+1);

		{
			//bottom		
			double drumDelta = middleHeightT / (double)drumHCount1;
			float offset = 3.1415926 / drumHCount1;
			for (size_t i = 0; i < drumHCount1; i++)
			{
				float delta = shape_bottom_height + i * drumDelta;
				float _offset = shape_middle_width * sin((offset * i) > 0 ? offset * i : 0);
				if (onePloy)
				{
					offsetPolyType(m_poly, _offset / 2, middlePolys.at(i),m_adParam.bluntSharpCorners);
					setPolyTreeZ(middlePolys.at(i), delta);
				}
				else
				{
					offsetAndExtendpolyType(m_poly, _offset / 2, thickness, delta, middlePolys.at(i),m_adParam.bluntSharpCorners);
					_simplifyPoly(&middlePolys.at(i));
				}
			}
		}

		{
			//top
			size_t drumHCount2 = drumHCount- drumHCount1;
			double drumDelta = middleHeightB / (double)drumHCount2;
			float offset = 3.1415926 / drumHCount2;
			for (size_t i = 0; i < drumHCount2+1; i++)
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
					offsetAndExtendpolyType(m_poly, _offset / 2, thickness, delta, middlePolys.at(drumHCount1+ i),m_adParam.bluntSharpCorners);
					_simplifyPoly(&middlePolys.at(drumHCount1+ i));
				}
			}
		}

		for (size_t i = 0; i < middlePolys.size()-1; i++)
		{
			//if (onePloy)
			//{
			//	_buildFromDiffPolyTree_onePoly(&middlePolys.at(i), &middlePolys.at(i + 1));
			//}
			//else
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