#include "slotgenerator.h"

namespace fmesh
{
	SlotGenerator::SlotGenerator()
	{

	}

	SlotGenerator::~SlotGenerator()
	{

	}

	void SlotGenerator::build()
	{
		ClipperLib::PolyTree treeTop, treeBottom;
		double hTop, hBottom;

		_buildTop(treeTop, hTop);
		_buildBottom(treeBottom, hBottom);

		//_buildFromSamePolyTree(&treeBottom, &treeTop);
	}
}