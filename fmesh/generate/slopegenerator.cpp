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
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys);
	}

	void SlopeGenerator::buildShell()
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys,true);
	}

	void SlopeGenerator::buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree)
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		if (middlePolys.size() == 0)
			return;
		offsetPolyType(middlePolys.front(), m_adParam.exoprtParam.bottom_offset, bottomTree, m_adParam.bluntSharpCorners);
	}

	void SlopeGenerator::buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys, bool onePoly)
	{
		double thickness = m_adParam.extend_width / 2.0;;

		middlePolys.resize(3);
		if (onePoly)
		{
			copy2PolyTree(m_poly, middlePolys.at(0));
			copy2PolyTree(m_poly, middlePolys.at(1));
			setPolyTreeZ(middlePolys.at(1), m_adParam.shape_bottom_height);
			copy2PolyTree(m_poly, middlePolys.at(2));
			setPolyTreeZ(middlePolys.at(2), m_adParam.shape_bottom_height + 1);
		} 
		else
		{
			offsetAndExtendpolyType(m_poly, 0.0, thickness, 0, middlePolys.at(0),m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, 0.0, thickness, m_adParam.shape_bottom_height, middlePolys.at(1), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, 0.0, thickness, m_adParam.shape_bottom_height + 1, middlePolys.at(2), m_adParam.bluntSharpCorners);
		}

		if (m_adParam.total_height < m_adParam.shape_bottom_height)
		{
			if (onePoly)
			{
				copy2PolyTree(m_poly, middlePolys.at(1));
				passivationPolyTree(middlePolys.at(1), thickness / 2.0);
				setPolyTreeZ(middlePolys.at(1), m_adParam.total_height);
			}
			else
			{
				offsetAndExtendpolyType(m_poly, 0.0, thickness, m_adParam.total_height, middlePolys.at(1),m_adParam.bluntSharpCorners);
			}

			_buildFromSamePolyTree(&middlePolys.at(0), &middlePolys.at(1));
	
			_fillPolyTree(&middlePolys.at(0));
			_fillPolyTree(&middlePolys.at(1));
			return;
		}

		double slope = (m_adParam.total_height - m_adParam.shape_bottom_height - 1) / (dmax.y - dmin.y);
		_dealPolyTreeAxisZ(&middlePolys.at(2), slope, dmin.y);

		if (onePoly)
		{
			_buildFromSamePolyTree(&middlePolys.at(0), &middlePolys.at(1));
			_buildFromSamePolyTree(&middlePolys.at(1), &middlePolys.at(2));
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