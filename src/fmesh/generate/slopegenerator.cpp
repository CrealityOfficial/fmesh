#include "slopegenerator.h"

namespace fmesh
{
	SlopeGenerator::SlopeGenerator()
	{

	}

	SlopeGenerator::~SlopeGenerator()
	{

	}

	void SlopeGenerator::build()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);
	}

	void SlopeGenerator::buildShell()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys,true);
	}

	void SlopeGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);
		bottomTree = middlePolys.front();
	}

	void SlopeGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePoly)
	{
		double thickness = m_adParam.extend_width / 2.0;;

		middlePolys.resize(3);
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, 0, middlePolys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_adParam.shape_bottom_height, middlePolys.at(1));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_adParam.shape_bottom_height + 1, middlePolys.at(2));

		if (m_adParam.total_height < m_adParam.shape_bottom_height)
		{
			offsetAndExtendPolyTree(m_poly, 0.0, thickness, m_adParam.total_height, middlePolys.at(1));
			if (onePoly)
				_buildFromSamePolyTree(&middlePolys.at(0), &middlePolys.at(1),3);//outer
			else
				_buildFromSamePolyTree(&middlePolys.at(0), &middlePolys.at(1));
	
			_fillPolyTree(&middlePolys.at(0));
			_fillPolyTree(&middlePolys.at(1));
			return;
		}

		double slope = (m_adParam.total_height - m_adParam.shape_bottom_height - 1) / (dmax.y - dmin.y);
		_dealPolyTreeAxisZ(&middlePolys.at(2), slope, dmin.y);

		if (onePoly)
		{
			_buildFromSamePolyTree(&middlePolys.at(0), &middlePolys.at(1),3);
			_buildFromSamePolyTree(&middlePolys.at(1), &middlePolys.at(2),3);
		} 
		else
		{
			_buildFromSamePolyTree(&middlePolys.at(0), &middlePolys.at(1));
			_buildFromSamePolyTree(&middlePolys.at(1), &middlePolys.at(2));
			_fillPolyTree(&middlePolys.front());
			_fillPolyTree(&middlePolys.back());
		}

	}

	void SlopeGenerator::initTestData()
	{

	}

}