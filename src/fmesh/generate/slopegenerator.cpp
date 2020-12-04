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
		double thickness = m_param.thickness / 4.0f;

		std::vector<ClipperLib::PolyTree> polys(3);
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, 0, polys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_param.slopeRightH, polys.at(1));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_param.slopeRightH+1, polys.at(2));

		double slope = (m_param.totalH - m_param.slopeRightH-1)/ (m_modelparam.dmax.y - m_modelparam.dmin.y);
 		_dealPolyTreeAxisZ(&polys.at(2), slope, m_modelparam.dmin.y);

		_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
		_buildFromSamePolyTree(&polys.at(1), &polys.at(2));
		_fillPolyTree(&polys.front());
		_fillPolyTree(&polys.back());
	}
}