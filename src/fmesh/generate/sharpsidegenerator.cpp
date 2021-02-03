#include "sharpsidegenerator.h"

namespace fmesh
{
	SharpsideGenerator::SharpsideGenerator()
	{

	}

	SharpsideGenerator::~SharpsideGenerator()
	{

	}

	void SharpsideGenerator::build()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);

		_buildTopBottom(&middlePolys.front(), &middlePolys.back());
	}

	void SharpsideGenerator::buildShell()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys,true);
		_buildTopBottom_onepoly(&middlePolys.front(), &middlePolys.back());
	}

	void SharpsideGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);

		topTree = middlePolys.back();
		bottomTree = middlePolys.front();
		_buildBoardPoly(&topTree);
		_buildBoardPoly(&bottomTree);
	}

	void SharpsideGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePoly)
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
		if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
			shape_bottom_height = m_adParam.shape_bottom_height > m_adParam.bottom_height ? m_adParam.shape_bottom_height : m_adParam.bottom_height;
		}

		float middleHeight = m_adParam.total_height - shape_bottom_height - shape_top_height;

		size_t drumHCount = middleHeight / 0.5;
		double drumDelta = middleHeight / (double)drumHCount;
		middlePolys.resize(1 + drumHCount);

		float offset = 3.1415926 / drumHCount;
		float offsetbottom = middleHeight / 2 / drumHCount;
		float offsettop = middleHeight / 2 / drumHCount;

		size_t middle = drumHCount / 2;
		std::vector<float> offs(drumHCount + 1);
		for (size_t i = 0; i < drumHCount + 1; i++)
		{
			float delta = shape_bottom_height + i * drumDelta;

			float _offset = 0.0f;
			if (i > middle)
			{
				_offset = 2 * shape_middle_width - 2 * i * shape_middle_width / drumHCount;

			}
			else
			{
				_offset = i * 2 * shape_middle_width / drumHCount;;
			}
			offsetAndExtendPolyTree(m_poly, _offset / 2, thickness, delta, middlePolys.at(i));
			offs.at(i) = _offset;
		}

		for (size_t i = 0; i < middlePolys.size() - 1; i++)
		{
			if (onePoly)
				_buildFromDiffPolyTree_diffSafty(&middlePolys.at(i), &middlePolys.at(i + 1), 1.0, 3);
			else
				_buildFromDiffPolyTree_diffSafty(&middlePolys.at(i), &middlePolys.at(i + 1));
		}
	}

	void SharpsideGenerator::initTestData()
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