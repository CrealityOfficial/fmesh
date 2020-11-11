#include "stepgenerator.h"
#include "fmesh/contour/contour.h"
namespace fmesh
{
	StepGenerator::StepGenerator()
	{

	}

	StepGenerator::~StepGenerator()
	{

	}

	void StepGenerator::build()
	{
		double thickness = m_param.thickness / 2.0f;
		double offset = -m_param.bottomStepW;
		std::vector<float> heights(5);
		heights.at(0) = 0.0f;
		heights.at(1) = m_param.bottomStepH;
		heights.at(2) = m_param.bottomStepH + 0.5f;
		heights.at(3) = m_param.bottomStepH + 0.5f;
		heights.at(4) = m_param.totalH;

		std::vector<ClipperLib::PolyTree> polys(5);

		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(0), polys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(1), polys.at(1));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(2), polys.at(2));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(3), polys.at(3));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(4), polys.at(4));
		offsetExteriorInner(polys.at(2), offset);

		_fillPolyTree(&polys.at(0), true);
		_fillPolyTree(&polys.at(4));	
		
		_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
		_buildFromDiffPolyTree(&polys.at(1), &polys.at(2));
		_buildFromDiffPolyTree(&polys.at(2), &polys.at(3));
		_buildFromSamePolyTree(&polys.at(3), &polys.at(4));
	}
}