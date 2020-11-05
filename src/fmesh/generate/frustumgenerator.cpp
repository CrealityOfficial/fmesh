#include "frustumgenerator.h"

namespace fmesh
{
	FrustumGenerator::FrustumGenerator()
	{

	}

	FrustumGenerator::~FrustumGenerator()
	{

	}

	void FrustumGenerator::build()
	{
		double thickness = m_param.thickness / 2.0f;
		int count = 10;
		double offset = m_param.totalH * std::sin(11.0 * 3.1415926 / 180.0) / (double)count;
		double h = m_param.totalH / (double)count;
		std::vector<ClipperLib::PolyTree> polys(count + 1);
		for (int i = 0; i <= count; ++i)
		{
			fmesh::offsetAndExtendPolyTree(m_poly, (double)i * offset, thickness, (double)i * h, polys.at(i));
		}
		for (int i = 0; i < count; ++i)
		{
			ClipperLib::PolyTree out;
			fmesh::xor2PolyTrees(&polys.at(i + 1), &polys.at(i), out);
			_fillPolyTree(&out);
		}
		_fillPolyTree(&polys.front());
		_fillPolyTree(&polys.back());
	}
}