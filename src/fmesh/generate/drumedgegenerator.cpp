#include "drumedgegenerator.h"

namespace fmesh
{
	DrumedgeGenerator::DrumedgeGenerator()
	{

	}

	DrumedgeGenerator::~DrumedgeGenerator()
	{

	}

	void DrumedgeGenerator::build()
	{
		//test data

		//
		float shape_bottom_height= m_adParam.shape_bottom_height;
		float shape_top_height= m_adParam.shape_top_height;
		float shape_middle_width= m_adParam.shape_middle_width;
		float thickness = m_adParam.extend_width / 4.0;

		//int count = 10;
		float middleHeight = m_adParam.total_height - shape_bottom_height - shape_top_height;
		//float h = middleHeight / (float)count;

		std::vector<ClipperLib::PolyTree> polys(4);
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, 0, polys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, shape_bottom_height, polys.at(1));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_adParam.total_height - shape_top_height, polys.at(2));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_adParam.total_height, polys.at(3));

		size_t drumHCount = middleHeight / 0.5;
		std::vector<ClipperLib::PolyTree> middlePolys(1 + drumHCount);

		float offset = 3.1415926 / drumHCount;

		size_t middle = drumHCount / 2;
		std::vector<float> offs(drumHCount + 1);
		for (size_t i = 0; i < drumHCount + 1; i++)
		{
			float delta = shape_bottom_height + i * 0.5;
			float _offset = shape_middle_width*sin((offset*i)>0? offset * i:0);
			offsetAndExtendPolyTree(m_poly, _offset / 2, thickness, delta, middlePolys.at(i));
			offs.at(i) = _offset;
		}

		for (size_t i = 0; i < middlePolys.size()-1; i++)
		{
			ClipperLib::PolyTree out;
			_buildFromDiffPolyTree_drum(&middlePolys.at(i), &middlePolys.at(i + 1),0,out);
			if (out.ChildCount() > 0)
			{
				_fillPolyTreeReverseInner(&out, offs.at(i) < offs.at(i + 1));
			}
		}

		_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
		_buildFromSamePolyTree(&polys.at(2), &polys.at(3));
		_fillPolyTree(&polys.front(),true);
		_fillPolyTree(&polys.back());
	}
}