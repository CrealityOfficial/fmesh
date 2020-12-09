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
		float shape_bottom_height= m_adParam.shape_bottom_height;
		float shape_top_height= m_adParam.shape_top_height;
		float shape_middle_width= m_adParam.shape_middle_width;
		float thickness = m_adParam.extend_width / 4.0;

		float middleHeight = m_adParam.total_height - shape_bottom_height - shape_top_height;

		std::vector<ClipperLib::PolyTree> polys(5);
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, 0, polys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, shape_bottom_height, polys.at(1));
		offsetAndExtendPolyTree(m_poly, shape_middle_width / 2.0, thickness, shape_bottom_height + middleHeight / 2.0, polys.at(2));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_adParam.total_height - shape_top_height, polys.at(3));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_adParam.total_height, polys.at(4));

		_buildFromDiffPolyTree(&polys.at(1), &polys.at(2));
		_buildFromDiffPolyTree(&polys.at(2), &polys.at(3));
		_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
		_buildFromSamePolyTree(&polys.at(3), &polys.at(4));
		_fillPolyTree(&polys.front(),true);
		_fillPolyTree(&polys.back());
	}
}