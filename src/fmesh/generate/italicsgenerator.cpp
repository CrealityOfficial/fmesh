#include "italicsgenerator.h"

namespace fmesh
{
	ItalicsGenerator::ItalicsGenerator()
	{

	}

	ItalicsGenerator::~ItalicsGenerator()
	{

	}

	void ItalicsGenerator::build()
	{
		double thickness = m_param.thickness / 4.0f;
		int count = 10;
		double offset = m_param.totalH * std::sin(m_param.italicsAngle/2 * 3.1415926 / 180.0) / (double)count;
		double h = m_param.totalH / (double)count;
		std::vector<ClipperLib::PolyTree> polys(count + 1);
		//offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_param.italicsBottomH, polys.at(0));
		for (int i = 0; i <= count; ++i)
		{
			fmesh::offsetAndExtendPolyTree(m_poly, -(double)i * offset, thickness, (double)i * h, polys.at(i));
		}
		for (int i = 0; i < count; ++i)
		{
			ClipperLib::PolyTree out;
			_buildFromDiffPolyTree_drum(&polys.at(i), &polys.at(i + 1), 0, out);
			if (out.ChildCount() > 0)
			{
				_fillPolyTreeReverseInner(&out);
			}
		}
		_fillPolyTree(&polys.front());
		_fillPolyTree(&polys.back());
	}
}