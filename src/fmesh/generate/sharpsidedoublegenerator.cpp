#include "sharpsidedoublegenerator.h"

namespace fmesh
{
	SharpsideDoubleGenerator::SharpsideDoubleGenerator()
	{

	}

	SharpsideDoubleGenerator::~SharpsideDoubleGenerator()
	{

	}

	void SharpsideDoubleGenerator::build()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);

		_buildTopBottomDiff(&middlePolys.front(), &middlePolys.back());
	}

	void SharpsideDoubleGenerator::buildShell()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys,true);
		_buildTopBottom_onepoly(&middlePolys.front(), &middlePolys.back());
	}

	void SharpsideDoubleGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);

		copy2PolyTree(middlePolys.back(), topTree);
		copy2PolyTree(middlePolys.front(), bottomTree);
		//_buildBoardPoly(&topTree);
		//_buildBoardPoly(&bottomTree);
	}

	void SharpsideDoubleGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePoly)
	{
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
		size_t drumHCount = middleHeight / 0.5;
		middlePolys.resize(drumHCount+1);

		float middleHeightT = middleHeight / 2;
		float middleHeightB = middleHeight / 2;
		size_t drumHCountT = drumHCount / 2;
		size_t drumHCountB = drumHCount / 2;
		{
			//1
			double drumDelta = middleHeightT / (double)drumHCountT;
			float offset = 3.1415926 / drumHCountT;
			float offsetbottom = middleHeightT / 2 / drumHCountT;
			float offsettop = middleHeightT / 2 / drumHCountT;

			size_t middle = drumHCountT / 2;
			for (size_t i = 0; i < drumHCountT; i++)
			{
				float delta = shape_bottom_height + i * drumDelta;

				float _offset = 0.0f;
				if (i > middle)
				{
					_offset = 2 * shape_middle_width - 2 * i * shape_middle_width / drumHCountT;

				}
				else
				{
					_offset = i * 2 * shape_middle_width / drumHCountT;;
				}
				if (onePoly)
				{
					offsetPolyType(m_poly, _offset / 2, middlePolys.at(i),m_adParam.bluntSharpCorners);
					passivationPolyTree(middlePolys.at(i), thickness / 2.0);
					setPolyTreeZ(middlePolys.at(i), delta);
				}
				else
				{
					offsetAndExtendpolyType(m_poly, _offset / 2, thickness, delta, middlePolys.at(i),m_adParam.bluntSharpCorners);
				}
			}
		}
		{
			//2
			double drumDelta = middleHeightB / (double)drumHCountB;
			float offset = 3.1415926 / drumHCountB;
			float offsetbottom = middleHeightB / 2 / drumHCountB;
			float offsettop = middleHeightB / 2 / drumHCountB;

			size_t middle = drumHCountB / 2;
			for (size_t i = 0; i < drumHCountB+1; i++)
			{
				float delta = shape_bottom_height + (drumHCountT+i) * drumDelta;

				float _offset = 0.0f;
				if (i > middle)
				{
					_offset = 2 * shape_middle_width - 2 * i * shape_middle_width / drumHCountB;

				}
				else
				{
					_offset = i * 2 * shape_middle_width / drumHCountB;;
				}
				if (onePoly)
				{
					offsetPolyTree(m_poly, _offset / 2, middlePolys.at(drumHCountT + i));
					passivationPolyTree(middlePolys.at(drumHCountT + i), thickness / 2.0);
					setPolyTreeZ(middlePolys.at(drumHCountT + i), delta);
				}
				else
				{
					offsetAndExtendpolyType(m_poly, _offset / 2, thickness, delta, middlePolys.at(drumHCountT + i),m_adParam.bluntSharpCorners);
				}
			}
		}

		for (size_t i = 0; i < middlePolys.size()-1; i++)
		{
			//if (onePoly)
			//	_buildFromDiffPolyTree_onePoly(&middlePolys.at(i), &middlePolys.at(i + 1));
			//else
				_buildFromDiffPolyTree_diffSafty(&middlePolys.at(i), &middlePolys.at(i + 1));
		}
	}

	void SharpsideDoubleGenerator::initTestData()
	{
		m_adParam.top_type = ADTopType::adtt_step;
		m_adParam.top_height = 1.0;
		m_adParam.shape_top_height = 2.0;
		m_adParam.bottom_type = ADBottomType::adbt_close;
		m_adParam.bottom_height = 1.0;
		m_adParam.shape_bottom_height = 3.0;
		m_adParam.shape_middle_width = 6.0;
	}

}