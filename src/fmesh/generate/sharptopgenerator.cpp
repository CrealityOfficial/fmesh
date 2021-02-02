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
		buildMiddle(middlePolys);

		m_adParam.bottom_type = ADBottomType::adbt_none;
		m_adParam.top_type = ADTopType::adtt_none;
		_buildTopBottom(&middlePolys.front(), nullptr);
	}

	void SharptopGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);

		//topTree = middlePolys.back();
		bottomTree = middlePolys.front();
	}

	void SharptopGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys)
	{

		double thickness = m_adParam.extend_width / 2.0;
		double bottomHeight = m_adParam.total_height - m_adParam.shape_top_height;

		//bottom
		middlePolys.resize(1);
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, bottomHeight, middlePolys.at(0));

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