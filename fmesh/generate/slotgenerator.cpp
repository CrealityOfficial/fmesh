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
		ClipperLib::PolyTree treeTop, treeBottom,treeMiddle;
		double hTop, hBottom,hMiddle;

		_buildTop(treeTop, hTop);
		_buildBottom(treeBottom, hBottom);
		
		//step start
		std::vector<ClipperLib::PolyTree> Steppolys(4);
		offsetAndExtendpolyType(m_poly, 0, m_adParam.extend_width / 2.0, m_adParam.shape_bottom_height, Steppolys.at(0),m_adParam.bluntSharpCorners);
		offsetAndExtendpolyType(m_poly, 0, m_adParam.extend_width / 2.0, m_adParam.shape_bottom_height, Steppolys.at(1), m_adParam.bluntSharpCorners);
		offsetAndExtendpolyType(m_poly, 0, m_adParam.extend_width / 2.0, m_adParam.total_height - m_adParam.shape_top_height, Steppolys.at(2), m_adParam.bluntSharpCorners);
		offsetAndExtendpolyType(m_poly, 0, m_adParam.extend_width / 2.0, m_adParam.total_height - m_adParam.shape_top_height, Steppolys.at(3), m_adParam.bluntSharpCorners);
		offsetExteriorInner(Steppolys.at(1), m_adParam.bottom_extend_width, m_adParam.shape_bottom_height);
		offsetExteriorInner(Steppolys.at(2), m_adParam.bottom_extend_width, m_adParam.total_height - m_adParam.shape_top_height);
		_buildFromDiffPolyTree(&Steppolys.at(0), &Steppolys.at(1));
		_buildFromDiffPolyTree(&Steppolys.at(2), &Steppolys.at(3));
		_buildFromSamePolyTree(&Steppolys.at(1), &Steppolys.at(2));	
		_buildFromSamePolyTree(&Steppolys.back(), &treeTop);
		_buildFromSamePolyTree(&treeBottom, &Steppolys.front());
		//step end
	}
}