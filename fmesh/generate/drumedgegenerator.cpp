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
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys);
		_buildTopBottomDiff(&middlePolys.front(), &middlePolys.back());
	}

	void DrumedgeGenerator::buildShell()
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys,true);
		_buildTopBottom_onepoly(&middlePolys.front(), &middlePolys.back());
	}

	void DrumedgeGenerator::buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree)
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);

		offsetPolyType(middlePolys.back(), m_adParam.exoprtParam.top_offset, topTree, m_adParam.bluntSharpCorners);
		offsetPolyType(middlePolys.front(), m_adParam.exoprtParam.bottom_offset, bottomTree, m_adParam.bluntSharpCorners);
	}

	void DrumedgeGenerator::buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys, bool onePloy)
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
				offsetAndExtendpolyType(m_poly, _offset / 2, thickness, delta, middlePolys.at(i),m_adParam.bluntSharpCorners);
				_simplifyPoly(&middlePolys.at(i));
			}
			offs.at(i) = _offset;
		}

		for (size_t i = 0; i < middlePolys.size()-1; i++)
		{
			//if (onePloy)
			//{
				//_buildFromDiffPolyTree_onePoly(&middlePolys.at(i), &middlePolys.at(i + 1));
			//}
			//else
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