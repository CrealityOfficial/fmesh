#include "simplegenerator.h"
#include "fmesh/generate/triangularization.h"

namespace fmesh
{
	SimpleGenerator::SimpleGenerator()
	{

	}

	SimpleGenerator::~SimpleGenerator()
	{

	}

	void SimpleGenerator::build()
	{
		ClipperLib::PolyTree poly0, poly1;
		double thickness = m_param.thickness / 2.0f;
		extendPolyTree(m_poly, thickness, poly0);

		auto f = [this](const ClipperLib::IntPoint& point)->ClipperLib::IntPoint {
			ClipperLib::IntPoint p = point;
			p.Z += (ClipperLib::cInt)(1000.0 * m_param.totalH);
			return p;
		};
		extendPolyTree(m_poly, thickness, f, poly1);
		_buildFromSamePolyTree(&poly0, &poly1);
	}
}