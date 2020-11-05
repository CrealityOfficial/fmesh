#include "bottomgenerator.h"

namespace fmesh
{
	BottomGenerator::BottomGenerator()
	{

	}

	BottomGenerator::~BottomGenerator()
	{

	}

	void BottomGenerator::build()
	{
		ClipperLib::PolyTree poly0, polyH, polyB;
		double thickness = m_param.thickness / 2.0f;
		extendPolyTree(m_poly, thickness, poly0);

		auto fNormalB = [this](const ClipperLib::IntPoint& point)->ClipperLib::IntPoint {
			ClipperLib::IntPoint p = point;
			p.Z += (ClipperLib::cInt)(1000.0 * m_param.bottomH);
			return p;
		};
		auto fNormalH = [this](const ClipperLib::IntPoint& point)->ClipperLib::IntPoint {
			ClipperLib::IntPoint p = point;
			p.Z += (ClipperLib::cInt)(1000.0 * m_param.totalH);
			return p;
		};
		extendPolyTree(m_poly, thickness, fNormalH, polyH);
		extendPolyTree(m_poly, thickness, fNormalB, polyB);

		_fillPolyTree(&polyH);
		_fillPolyTreeOutline(&poly0, true);
		_fillPolyTreeInner(&polyB);
		_buildFromSamePolyTree(&polyB, &polyH);
		_buildFromSamePolyTree(&poly0, &polyB, 1);
	}
}