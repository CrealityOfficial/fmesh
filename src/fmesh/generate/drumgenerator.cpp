#include "drumgenerator.h"
#include "fmesh/contour/contour.h"
#include <clipper/clipper.hpp>
#include "mmesh/clipper/circurlar.h"

namespace fmesh
{
	DrumGenerator::DrumGenerator()
	{

	}

	DrumGenerator::~DrumGenerator()
	{

	}

	void DrumGenerator::build()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);
		_buildTopBottom(&middlePolys.front(),nullptr);
	}

	void DrumGenerator::buildShell()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys,true);
		_buildTopBottom_onepoly(&middlePolys.front(), nullptr);
	}

	void DrumGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		copy2PolyTree(middlePolys.front(), bottomTree);
		//_buildBoardPoly(&bottomTree);
	}

	void DrumGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys,bool onePloy)
	{
		double thickness = m_adParam.extend_width / 2.0;

		double x = dmax.x - dmin.x;
		double y = dmax.y - dmin.y;

		float offsetH = 0.4;
		size_t drumHCount = 150;

		float offsetr = 1.0 * m_adParam.shape_top_height / drumHCount;
		middlePolys.resize(1 + drumHCount);
		for (size_t i = 0; i < drumHCount; i++)
		{
			float _offsetr = m_adParam.shape_top_height - sqrt(pow(m_adParam.shape_top_height, 2) - pow(i * offsetr, 2));

			if (onePloy)
			{
				offsetPolyTree(m_poly, -_offsetr * 50, middlePolys.at(i));
				//setPolyTreeZ(middlePolys.at(i), delta);
			} 
			else
			{
				offsetAndExtendPolyTree(m_poly, -_offsetr * 50, thickness, middlePolys.at(i));
				_simplifyPoly(&middlePolys.at(i));
			}
			if (i && GetPolyCount(&middlePolys.at(i)) != GetPolyCount(&middlePolys.at(i - 1)))
			{
				middlePolys.at(i).Clear();
				break;
			}
		}

		while (!middlePolys.back().ChildCount())
		{
			middlePolys.pop_back();
		}

		float delta1 = 0.0f;
		float delta2 = 0.0f;
		double bottomHeight = m_adParam.total_height - (middlePolys.size() - 1) * offsetH;
		if (bottomHeight < 0)
			bottomHeight = 0;

		for (size_t i = 0; i < middlePolys.size() - 1; i++)
		{
			delta1 = bottomHeight + offsetH * i;
			delta2 = bottomHeight + offsetH * (i + 1);
			if (i % 2)
				setPolyTreeZ(middlePolys.at(i), delta1);
			setPolyTreeZ(middlePolys.at(i + 1), delta2);
			
			if (onePloy)
				_buildFromDiffPolyTree_onePoly(&middlePolys.at(i), &middlePolys.at(i + 1));//outer
			else
				_buildFromDiffPolyTree_diffSafty(&middlePolys.at(i), &middlePolys.at(i + 1));
		}

		std::vector<Patch*> patches;
		skeletonPolyTree(middlePolys.back(), delta2, patches, middlePolys.size() / 100.0, onePloy);
		addPatches(patches);
		//_fillPolyTree(&middlePolys.back(), true);
		m_adParam.bottom_height = bottomHeight;
	}

	void DrumGenerator::initTestData()
	{

	}

}