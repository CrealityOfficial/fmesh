#include "sharptopgenerator.h"
#include "fmesh/contour/contour.h"
#include "mmesh/clipper/circurlar.h"

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
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);
		_buildTopBottom(&middlePolys.front(), nullptr);
	}

	void SharptopGenerator::buildShell()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys,true);
		_buildTopBottom_onepoly(&middlePolys.front(), nullptr);
	}

	void SharptopGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		copy2PolyTree(middlePolys.front(), bottomTree);
	}

	void SharptopGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePoly)
	{

		double thickness = m_adParam.extend_width / 2.0;
		double bottomHeight = m_adParam.total_height - m_adParam.shape_top_height;

		//bottom
		middlePolys.resize(1);
		if (onePoly)
		{
			copy2PolyTree(m_poly,middlePolys.at(0));
			setPolyTreeZ(middlePolys.at(0), bottomHeight);
		} 
		else
		{
			offsetAndExtendPolyTree(m_poly, 0.0, thickness, bottomHeight, middlePolys.at(0));
		}

		_simplifyPoly(&middlePolys.back());

		std::vector<Patch*> patches;
		skeletonPolyTreeSharp(middlePolys.back(), bottomHeight, m_adParam.shape_top_height, patches);

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