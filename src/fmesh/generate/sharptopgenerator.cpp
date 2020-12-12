#include "sharptopgenerator.h"
#include "fmesh/contour/contour.h"
#include "fmesh/roof/roof.h"

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
		double thickness = m_adParam.extend_width / 2.0;
		double bottomHeight = m_adParam.total_height - m_adParam.shape_top_height;

		//bottom
		std::vector<ClipperLib::PolyTree> bottomPolys(2);
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, 0, bottomPolys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, bottomHeight, bottomPolys.at(1));

		std::vector<Patch*> patches;
		skeletonPolyTreeSharp(bottomPolys.back(), bottomHeight, m_adParam.shape_top_height, patches);
		addPatches(patches);

		_fillPolyTree(&bottomPolys.front(), true);
		_buildFromSamePolyTree(&bottomPolys.front(), &bottomPolys.back());
	}
}