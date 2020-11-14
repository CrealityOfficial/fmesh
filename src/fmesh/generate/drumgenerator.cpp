#include "drumgenerator.h"
#include "fmesh/contour/contour.h"

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
		double thickness = m_param.thickness / 2.0f;
		double offset = thickness;
		std::vector<float> heights(2);
		heights.at(0) = 0.0f;
		heights.at(1) = m_param.totalH-m_param.drumH;

		size_t drumHCount = m_param.drumH/0.5;
		std::vector<ClipperLib::PolyTree> polys(2+ drumHCount);

		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(0), polys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(1), polys.at(1));
		for (size_t i = 2; i < drumHCount+1; i++)
		{
			double delta = heights.at(1) + (i - 1) * 0.5;
			double offset = m_param.drumH - sqrt(m_param.drumH * m_param.drumH - 0.25*i*i);
			offsetAndExtendPolyTree(m_poly, -offset, thickness, delta, polys.at(i));
		}

		skeletonPolyTree(m_poly, heights.at(1) + (drumHCount - 1) * 0.5, polys.at(1 + drumHCount));

// 		ClipperLib::Paths* paths = m_fontCenter->getPath(charCode);
// 		m_builder->setPaths(paths);
// 		ClipperLib::PolyTree* poly = m_builder->polyTree();
// 		updatePolyTree(poly);
// 		home(polyTree2Box(poly));

		_fillPolyTree(&polys.at(0), true);
		_fillPolyTree(&polys.at( drumHCount+1));

		_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
		for (size_t i=1;i < polys.size()-1;i++)
		{
			_buildFromDiffPolyTree(&polys.at(i), &polys.at(i+1));
		}


// 		double thickness = m_param.thickness / 2.0f;
// 		double offset = thickness;
// 		std::vector<float> heights(4);
// 		heights.at(0) = 0.0f;
// 		heights.at(1) = m_param.exteriorH;
// 		heights.at(2) = m_param.exteriorH;
// 		heights.at(3) = m_param.totalH;
// 
// 		std::vector<ClipperLib::PolyTree> polys(4);
// 
// 		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(0), polys.at(0));
// 		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(1), polys.at(1));
// 		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(2), polys.at(2));
// 		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(3), polys.at(3));
// 		offsetExterior(polys.at(0), offset);
// 		offsetExterior(polys.at(1), offset);
// 
// 		//auto f = [this, &heights](const ClipperLib::IntPoint& point)->ClipperLib::IntPoint {
// 		//	ClipperLib::IntPoint p = point;
// 		//	p.Z += (ClipperLib::cInt)(1000.0 * heights.at(1));
// 		//	return p;
// 		//};
// 		//
// 		//polyTreeOffset(polys.at(1), f);
// 
// 		_fillPolyTree(&polys.at(0), true);
// 		_fillPolyTree(&polys.at(3));
// 
// 		ClipperLib::PolyTree out;
// 		fmesh::xor2PolyTrees(&polys.at(1), &polys.at(2), out);
// 
// 		_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
// 		_buildFromSamePolyTree(&polys.at(2), &polys.at(3));
// 
// 		_buildFromDiffPolyTree_firstLayer(&out);
	}
}