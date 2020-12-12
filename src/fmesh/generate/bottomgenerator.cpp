#include "bottomgenerator.h"

namespace fmesh
{
	BottomGenerator::BottomGenerator()
	{

	}

	BottomGenerator::~BottomGenerator()
	{

	}

	void BottomGenerator::build()
	{
		ClipperLib::PolyTree treeTop, treeBottom;
		double hTop, hBottom;

		_buildTop(treeTop, hTop);

		m_adParam.bottom_type = ADBottomType::adbt_close;
		_buildBottom(treeBottom, hBottom);

		_buildFromSamePolyTree(&treeBottom, &treeTop);
	}
}