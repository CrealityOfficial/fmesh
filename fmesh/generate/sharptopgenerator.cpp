#include "sharptopgenerator.h"
#include "fmesh/contour/contour.h"
#include "fmesh/clipper/circurlar.h"

namespace fmesh
{
	SharptopGenerator::SharptopGenerator()
	{

	}

	SharptopGenerator::~SharptopGenerator()
	{

	}

	void SharptopGenerator::build()
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys);
		if (middlePolys.size() == 0)
			return;
		_buildTopBottomDiff(&middlePolys.front(), nullptr);
	}

	void SharptopGenerator::buildShell()
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys,true);
		if (middlePolys.size() == 0)
			return;
		_buildTopBottom_onepoly(&middlePolys.front(), nullptr);
	}

	void SharptopGenerator::buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree)
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		if (middlePolys.size() == 0)
			return;
		offsetPolyType(middlePolys.front(), m_adParam.exoprtParam.bottom_offset, bottomTree, m_adParam.bluntSharpCorners);
	}

	void SharptopGenerator::buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys, bool onePoly)
	{

		double thickness = m_adParam.extend_width / 2.0;
		double bottomHeight = m_adParam.total_height - m_adParam.shape_top_height;

		//detect
		float offsetH = 0.4;
		size_t drumHCount = 800;
		int lastvalueX = 0;
		int lastvalueY = 0;
		std::vector<ClipperLibXYZ::PolyTree> _middlePolys;
		_middlePolys.resize(1 + drumHCount);
		for (int i = 0; i < drumHCount; i++)
		{
			//float _offsetr = m_adParam.shape_top_height - sqrt(pow(m_adParam.shape_top_height, 2) - pow(i * offsetr, 2));
			float _offsetr = 0.05f;
			offsetAndExtendPolyTreeMiter(m_poly, -0.05 * i, thickness, _middlePolys.at(i));
			if (i > 1)
			{
				ClipperLibXYZ::IntPoint P = getAABBvalue(&_middlePolys.at(i));
				ClipperLibXYZ::IntPoint P1 = getAABBvalue(&_middlePolys.at(i - 1));
				int _lastvalueX = std::abs(P.X - P1.X);
				int _lastvalueY = std::abs(P.Y - P1.Y);
				if (P.X < P1.X * 2 / 3 || P.Y < P1.Y * 2 / 3)
				{
					_middlePolys.at(i).Clear();
					_middlePolys.at(i - 1).Clear();
					break;
				}
				lastvalueX = _lastvalueX;
				lastvalueY = _lastvalueX;
			}

			if (i > 0 && GetPolyCount(&_middlePolys.at(i)) != GetPolyCount(&_middlePolys.at(i - 1)))
			{
				_middlePolys.at(i).Clear();
				break;
			}
		}
		while (_middlePolys.size() && !_middlePolys.back().ChildCount())
		{
			_middlePolys.pop_back();
		}

		if (_middlePolys.size())
		{
			_middlePolys.pop_back();
		}
		else
		{
			return;
		}

		//bottom
		middlePolys.resize(2);
		if (onePoly)
		{
			copy2PolyTree(m_poly,middlePolys.at(0));
			setPolyTreeZ(middlePolys.at(0), bottomHeight);
			copy2PolyTree(m_poly, middlePolys.at(1));
			setPolyTreeZ(middlePolys.at(1), bottomHeight);
		} 
		else
		{
			//offsetAndExtendPolyTree(m_poly, 0.0, thickness, bottomHeight, middlePolys.at(0));

			offsetAndExtendPolyTreeMiter(m_poly, 0.0, thickness, middlePolys.at(0));
			setPolyTreeZ(middlePolys.at(0), bottomHeight);
			offsetAndExtendPolyTreeMiter(m_poly, 0.0, thickness, middlePolys.at(1));
			setPolyTreeZ(middlePolys.at(1), bottomHeight);
		}

		_simplifyPoly(&middlePolys.back(),100);

		std::vector<Patch*> patches;
		double topHeight = _middlePolys.size() / 100 > m_adParam.shape_top_height ? _middlePolys.size() / 100 : m_adParam.shape_top_height;
		skeletonPolyTreeSharp(middlePolys.back(), bottomHeight, topHeight, patches, onePoly);

		addPatches(patches);
	}

	void SharptopGenerator::initTestData()
	{
		m_adParam.top_type = ADTopType::adtt_step;
		m_adParam.top_height = 1.0;
		m_adParam.shape_top_height = 2.0;
		m_adParam.bottom_type = ADBottomType::adbt_step;
		m_adParam.bottom_height = 1.0;
		m_adParam.shape_bottom_height = 3.0;
	}

}