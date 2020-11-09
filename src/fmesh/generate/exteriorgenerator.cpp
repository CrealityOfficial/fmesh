#include "exteriorgenerator.h"
#include "fmesh/contour/contour.h"
namespace fmesh
{
	ExteriorGenerator::ExteriorGenerator()
	{

	}

	ExteriorGenerator::~ExteriorGenerator()
	{

	}

	void ExteriorGenerator::build()
	{
		double thickness = m_param.thickness / 2.0f;
		double offset = thickness;
		std::vector<float> heights(4);
		heights.at(0) = 0.0f;
		heights.at(1) = m_param.exteriorH;
		heights.at(2) = m_param.exteriorH;
		heights.at(3) = m_param.totalH;

		std::vector<ClipperLib::PolyTree> polys(4);

		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(0), polys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(1), polys.at(1));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(2), polys.at(2));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(3), polys.at(3));
		offsetExterior(polys.at(0), offset);
		offsetExterior(polys.at(1), offset);

		auto f = [this, &heights](const ClipperLib::IntPoint& point)->ClipperLib::IntPoint {
			ClipperLib::IntPoint p = point;
			p.Z += (ClipperLib::cInt)(1000.0 * heights.at(1));
			return p;
		};

		polyTreeOffset(polys.at(1), f);

		_fillPolyTree(&polys.at(0), true);
		_fillPolyTree(&polys.at(3));	

		ClipperLib::PolyTree out;
		fmesh::xor2PolyTrees(&polys.at(1), &polys.at(2), out);

		_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
		_buildFromSamePolyTree(&polys.at(2), &polys.at(3));

		_buildFromDiffPolyTree_firstLayer(&out);
	}
}