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
// 		ClipperLib::PolyTree treeBottom;
// 		double hBottom;
// 		_buildBottom(treeBottom, hBottom);

		double thickness = m_adParam.extend_width / 2.0;
		double bottomHeight = m_adParam.total_height - m_adParam.shape_top_height;
		size_t drumHCount = (m_adParam.shape_top_height - 0.5) / 0.8;
		std::vector<ClipperLib::PolyTree> middlePolys(1 + drumHCount);
		float delta = 0.0f;
		for (size_t i = 0; i < drumHCount + 1; i++)
		{
			delta = bottomHeight + i * 0.8;
			double offset = m_adParam.shape_top_height - sqrt(m_adParam.shape_top_height * m_adParam.shape_top_height - thickness * thickness * i * i);
			offsetAndExtendPolyTree(m_poly, - offset, thickness, delta, middlePolys.at(i));
		}

		std::vector<Patch*> patches;
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

		//_buildFromSamePolyTree(&treeBottom, &middlePolys.front());

		_buildTopBottom(&middlePolys.front(),nullptr);
	}
}