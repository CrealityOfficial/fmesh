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
		double thickness = m_param.thickness / 5.0f;
		double offset = thickness;
		std::vector<float> heights(2);
		heights.at(0) = 0.0f;
		heights.at(1) = m_param.totalH-m_param.drumH;

		size_t drumHCount = m_param.drumH/0.5;
		std::vector<ClipperLib::PolyTree> polys(1+ drumHCount);

		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(0), polys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(1), polys.at(1));
		for (size_t i = 2; i < drumHCount+1; i++)
		{
			double delta = heights.at(1) + (i - 1) * 0.5;
			double offset = m_param.drumH - sqrt(m_param.drumH * m_param.drumH - 0.25*i*i);
			offsetAndExtendPolyTree(m_poly, -offset/2, thickness, delta, polys.at(i));
		}

		std::vector<Patch*> patches;
		ClipperLib::PolyTree ploy;
		offsetAndExtendPolyTree(m_poly, 0, 0.2, 0.5, ploy);
		skeletonPolyTree(polys.at(drumHCount-1), heights.at(1) + (drumHCount - 2) * 0.5, patches);
		//sort(patches);
		addPatches(patches);

		_fillPolyTree(&polys.at(0), true);
		//_fillPolyTree(&polys.at( drumHCount+1));

		_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
		for (size_t i=1;i < polys.size()-2;i++)
		{
			ClipperLib::PolyTree out;
			_buildFromDiffPolyTree_drum(&polys.at(i), &polys.at(i+1), 0,out);
			if (out.ChildCount()>0)
			{
				_fillPolyTree(&out);
			}
		}
	}
}