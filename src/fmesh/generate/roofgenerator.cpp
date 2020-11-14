#include "roofgenerator.h"
#include "fmesh/contour/contour.h"

namespace fmesh
{
	RoofGenerator::RoofGenerator()
	{

	}

	RoofGenerator::~RoofGenerator()
	{

	}

	void RoofGenerator::build()
	{
		double thickness = m_param.thickness / 2.0f;
		double roofHeight = 5.0 * thickness;
		std::vector<float> heights(2);
		heights.at(0) = 0.0f;
		heights.at(1) = m_param.totalH;

		std::vector<ClipperLib::PolyTree> polys(2);
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(0), polys.at(0));

		_fillPolyTree(&polys.at(0), true);
		_buildRoof(&polys.at(0), roofHeight, thickness);
	}
}