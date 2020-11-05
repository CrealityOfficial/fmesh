#include "fillgenerator.h"

namespace fmesh
{
	FillGenerator::FillGenerator()
	{

	}

	FillGenerator::~FillGenerator()
	{

	}

	void FillGenerator::build()
	{
		ClipperLib::PolyTree poly0;
		double thickness = m_param.thickness / 2.0f;
		extendPolyTree(m_poly, thickness, poly0);
		_fillPolyTree(&poly0);
	}
}