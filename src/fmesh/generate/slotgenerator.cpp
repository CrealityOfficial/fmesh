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
		
		std::vector<ClipperLib::PolyTree> botomSteppolys(3);
		fmesh::offsetAndExtendPolyTree(m_poly, 0, m_adParam.extend_width / 2.0, m_adParam.shape_bottom_height, botomSteppolys.at(0));
		fmesh::offsetAndExtendPolyTree(m_poly, 0, m_adParam.extend_width / 2.0, m_adParam.shape_bottom_height + 0.5, botomSteppolys.at(1));
		fmesh::offsetAndExtendPolyTree(m_poly, 0, m_adParam.extend_width / 2.0, m_adParam.shape_bottom_height + 0.5, botomSteppolys.at(2));
		offsetExteriorInner(botomSteppolys.at(1), -m_adParam.bottom_extend_width);
		_buildFromDiffPolyTree(&botomSteppolys.at(0), &botomSteppolys.at(1));
		_buildFromDiffPolyTree(&botomSteppolys.at(1), &botomSteppolys.at(2));
		_buildFromSamePolyTree(&botomSteppolys.at(2), &treeTop);

		_buildBottom(treeBottom, hBottom);

		_buildFromSamePolyTree(&treeBottom, &botomSteppolys.at(0));
	}
}