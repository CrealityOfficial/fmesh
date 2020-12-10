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
}