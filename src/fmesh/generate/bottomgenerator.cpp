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

		m_adParam.top_type = ADTopType::adtt_close;
		_buildTop(treeTop, hTop);
		_buildBottom(treeBottom, hBottom);

		//_buildFromSamePolyTree(&treeBottom, &treeTop);
	}
}