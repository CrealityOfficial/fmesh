#include "nestedgenerator.h"
#include "fmesh/clipper/circurlar.h"
#include "specialpoly.h"

namespace fmesh
{
	NestedGenerator::NestedGenerator()
	{

	}

	NestedGenerator::~NestedGenerator()
	{

	}

	void NestedGenerator::build()
	{
		m_adParam.bottom_type = fmesh::ADBottomType::adbt_close;

		std::vector<ClipperLibXYZ::PolyTree> middlePolysOuter;
		std::vector<ClipperLibXYZ::PolyTree> middlePolysInner;
		buildOuter(middlePolysOuter);
		_buildFromSamePolyTree(&middlePolysOuter.front(), &middlePolysOuter.back());

		buildInner(middlePolysInner);

	}

	void NestedGenerator::buildShell()
	{
		m_adParam.bottom_type = fmesh::ADBottomType::adbt_close;

		std::vector<ClipperLibXYZ::PolyTree> middlePolysOuter;
		std::vector<ClipperLibXYZ::PolyTree> middlePolysInner;
		buildOuter(middlePolysOuter,true);
		_buildFromSamePolyTree(&middlePolysOuter.front(), &middlePolysOuter.back());

		buildInner(middlePolysInner,true);
	}

	void NestedGenerator::buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree)
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolysOuter;
		buildOuter(middlePolysOuter, true);
		copy2PolyTree(middlePolysOuter.front(), bottomTree);

		offsetPolyType(middlePolysOuter.front(), m_adParam.exoprtParam.bottom_offset, bottomTree, m_adParam.bluntSharpCorners);
	}

	ClipperLibXYZ::cInt NestedGenerator::getAABB(ClipperLibXYZ::PolyTree& poly)
	{
		ClipperLibXYZ::IntPoint pointMax(-999999, -999999);
		ClipperLibXYZ::IntPoint pointMin(999999, 999999);

		polyNodeFunc func = [&pointMax, &pointMin](ClipperLibXYZ::PolyNode* node) {
			for (ClipperLibXYZ::IntPoint& p : node->Contour)
			{
				if (pointMax.X < p.X)
					pointMax = p;
				if (pointMin.X > p.X)
				{
					pointMin = p;
				}
			}
		};
		fmesh::loopPolyTree(func, &poly);

		return pointMax.X - pointMin.X;
	}

	void NestedGenerator::buildOuter(std::vector<ClipperLibXYZ::PolyTree>& middlePolys,bool onePoly)
	{
		middlePolys.resize(2);
		double hTop, hBottom;

		if (onePoly)
		{
			_buildBottom_onepoly(middlePolys.at(0), hBottom, 0);
			_buildTop_onepoly(middlePolys.at(1), hTop, 0);
		}
		else
		{
			_buildTop(middlePolys.at(1), hTop);
			_buildBottom(middlePolys.at(0), hBottom);
		}
	}

	void NestedGenerator::buildInner(std::vector<ClipperLibXYZ::PolyTree>& middlePolys,bool onePoly)
	{
		float thickness = m_adParam.extend_width / 2.0 ;
		ClipperLibXYZ::PolyTree inner;

		middlePolys.resize(3);

		ClipperLibXYZ::cInt len = getAABB(m_poly);
		len += m_adParam.bottom_extend_width * 10000;

		offsetPolyType(m_poly, -(thickness + m_adParam.top_extend_width), inner, m_adParam.bluntSharpCorners);

		polyNodeFunc func1 = [&len](ClipperLibXYZ::PolyNode* node) {
			for (ClipperLibXYZ::IntPoint& p : node->Contour)
			{
				p.X += len;
			}
		};
		fmesh::loopPolyTree(func1, &inner);

		offsetAndExtendpolyType(inner, 0, thickness, m_adParam.top_height, middlePolys.at(2), m_adParam.bluntSharpCorners);		

		offsetAndExtendpolyType(inner, 0, thickness, thickness, middlePolys.at(1), m_adParam.bluntSharpCorners);
		offsetAndExtendpolyType(inner, 0, thickness, 0, middlePolys.at(0), m_adParam.bluntSharpCorners);

		if (onePoly)
		{
			_buildFromSamePolyTree(&middlePolys.at(0), &middlePolys.at(2),3);
		}
		else
		{
			_fillPolyTree(&middlePolys.at(2));
			_fillPolyTreeDepth14(&middlePolys.at(0), true);
			_fillPolyTreeDepth23(&middlePolys.at(1));
			_buildFromSamePolyTree(&middlePolys.at(0), &middlePolys.at(1), 3);
			_buildFromSamePolyTree(&middlePolys.at(1), &middlePolys.at(2));
		}
	}

	void NestedGenerator::initTestData()
	{
		m_adParam.top_type = ADTopType::adtt_close;
		m_adParam.top_height = 5.0;
		m_adParam.shape_top_height = 2.0;
		m_adParam.top_extend_width = 0.5;
		m_adParam.bottom_type = ADBottomType::adbt_step;
		m_adParam.bottom_height = 1.0;
		m_adParam.bottom_extend_width = 0.5;
		m_adParam.shape_bottom_height = 3.0;
		m_adParam.total_height = 15;
	}
}