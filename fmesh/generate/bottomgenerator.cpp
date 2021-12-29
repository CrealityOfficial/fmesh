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

		m_adParam.bottom_type = ADBottomType::adbt_step;
		m_adParam.bottom_extend_width = 1.0f;
		_buildBottom(treeBottom, hBottom);

		//_buildFromSamePolyTree(&treeBottom, &treeTop);
	}
}