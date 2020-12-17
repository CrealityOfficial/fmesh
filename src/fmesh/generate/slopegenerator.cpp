#include "slopegenerator.h"
#include "fmesh/build/layer.h"

namespace fmesh
{
	SlopeGenerator::SlopeGenerator()
	{

	}

	SlopeGenerator::~SlopeGenerator()
	{

	}

	void SlopeGenerator::build()
	{
		double thickness = m_adParam.extend_width / 4.0;;

		std::vector<ClipperLib::PolyTree> polys(3);
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, 0, polys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_adParam.shape_bottom_height, polys.at(1));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_adParam.shape_bottom_height + 1, polys.at(2));

		double slope = (m_adParam.total_height - m_adParam.shape_bottom_height - 1) / (dmax.y - dmin.y);
		_dealPolyTreeAxisZ(&polys.at(2), slope);

		_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
		_buildFromSamePolyTree(&polys.at(1), &polys.at(2));
		_fillPolyTree(&polys.front());
		_fillPolyTree(&polys.back());
	}
}