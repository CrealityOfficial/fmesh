#include "simplegenerator.h"
#include "fmesh/generate/triangularization.h"

namespace fmesh
{
	SimpleGenerator::SimpleGenerator()
	{

	}

	SimpleGenerator::~SimpleGenerator()
	{

	}

	void SimpleGenerator::build()
	{
		ClipperLib::PolyTree treeTop, treeBottom;
		double hTop, hBottom;

		_buildTop(treeTop, hTop);
		_buildBottom(treeBottom, hBottom);

		_buildFromSamePolyTree(&treeBottom, &treeTop);
	}

	void SimpleGenerator::buildShell()
	{
		float thickness = m_adParam.extend_width / 2.0;

		std::vector<ClipperLib::PolyTree> polys(2);
		offsetAndExtendPolyTree(m_poly, 0, thickness, 0, polys.at(0));
		offsetAndExtendPolyTree(m_poly, 0, thickness, m_adParam.total_height,polys.at(1));
		_buildFromSamePolyTree(&polys.at(0), &polys.at(1),3);
	}

}