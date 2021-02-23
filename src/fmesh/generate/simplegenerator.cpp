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
		ClipperLib::PolyTree treeTop, treeBottom;
		double hTop, hBottom;

		_buildTop_onepoly(treeTop, hTop);
		_buildBottom_onepoly(treeBottom, hBottom);

		_buildFromSamePolyTree(&treeBottom, &treeTop);
	}

	void SimpleGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		double hTop, hBottom;
		_buildTop(topTree, hTop);
		_buildBottom(bottomTree, hBottom);
	}
}