#include "reitalicsgenerator.h"

namespace fmesh
{
	ReItalicsGenerator::ReItalicsGenerator()
	{

	}

	ReItalicsGenerator::~ReItalicsGenerator()
	{

	}

	void ReItalicsGenerator::build()
	{
		//init
		float btoomStepHeight = m_adParam.shape_bottom_height;
		float bottomHeight = m_adParam.bottom_height;
		float thickness = m_adParam.extend_width / 2.0;
		float topHeight = m_adParam.top_height;
		float topStepWiden = m_adParam.top_extend_width / 2.0;
		float btoomStepWiden = m_adParam.bottom_extend_width / 2.0;
		float middleHeight = m_adParam.total_height - bottomHeight;
		size_t count = middleHeight /0.5;
		float offset = 0.5 * tan(m_adParam.shape_angle)/4;

		//bottom
		std::vector<ClipperLib::PolyTree> polys(count + 1);
		for (int i = 0; i <= count; ++i)
		{
			fmesh::offsetAndExtendPolyTree(m_poly, offset*i, thickness, middleHeight - (float)i * 0.5, polys.at(i));
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

		//top
		std::vector<ClipperLib::PolyTree> topPolys(1);
		fmesh::offsetAndExtendPolyTree(m_poly, 0, thickness, m_adParam.total_height, topPolys.at(0));

		_fillPolyTree(&topPolys.back());
		_fillPolyTree(&polys.back(),true);
		_buildFromSamePolyTree(&polys.front(), &topPolys.front());
	}
}