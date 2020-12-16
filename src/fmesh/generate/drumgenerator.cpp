#include "drumgenerator.h"
#include "fmesh/contour/contour.h"
#include "fmesh/roof/roof.h"

namespace fmesh
{
	DrumGenerator::DrumGenerator()
	{

	}

	DrumGenerator::~DrumGenerator()
	{

	}

	void DrumGenerator::build()
	{
		//test data
// 		m_adParam.top_type = ADTopType::adtt_step;
// 		m_adParam.top_height = 1.0;
// 		m_adParam.shape_top_height = 2.0;
// 		m_adParam.bottom_type = ADBottomType::adbt_step;
// 		m_adParam.bottom_height = 1.0;
// 		m_adParam.shape_bottom_height = 3.0;
		//

		double thickness = m_adParam.extend_width / 2.0;
		double bottomHeight = m_adParam.total_height - m_adParam.shape_top_height;

		//top
		size_t drumHCount = (m_adParam.shape_top_height-0.5) / 0.8;
		std::vector<ClipperLib::PolyTree> middlePolys(1 + drumHCount);
		float delta=0;
		for (size_t i = 0; i < drumHCount + 1; i++)
		{
			delta = bottomHeight + i * 0.8;
			double offset = m_adParam.shape_top_height - sqrt(m_adParam.shape_top_height * m_adParam.shape_top_height - thickness * thickness * i * i);
			offsetAndExtendPolyTree(m_poly, -offset, thickness, delta, middlePolys.at(i));
		}

		std::vector<Patch*> patches;
		ClipperLib::PolyTree ploy;
		offsetAndExtendPolyTree(m_poly, 0, 0.2, 0.5, ploy);
		skeletonPolyTree(middlePolys.back(), delta, patches);
		addPatches(patches);

		for (size_t i = 0; i < middlePolys.size() - 1; i++)
		{
			ClipperLib::PolyTree out;
			_buildFromDiffPolyTree_drum(&middlePolys.at(i), &middlePolys.at(i + 1), 0, out);
			if (out.ChildCount() > 0)
			{
				_fillPolyTreeReverseInner(&out, false);
			}
		}

		ClipperLib::PolyTree treeBottom;
		double hBottom;
		_buildBottom(treeBottom, hBottom);
		_buildFromSamePolyTree(&treeBottom, &middlePolys.front());
	}
}