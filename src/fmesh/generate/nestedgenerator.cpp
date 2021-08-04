#include "nestedgenerator.h"
#include "mmesh/clipper/circurlar.h"
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
		buildOuter();
		buildInner();
	}

	void NestedGenerator::buildShell()
	{
		buildOuter(true);
		buildInner(true);
	}

	void NestedGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{

	}

	ClipperLib::cInt NestedGenerator::getAABB(ClipperLib::PolyTree& poly)
	{
		ClipperLib::IntPoint pointMax(-999999, -999999);
		ClipperLib::IntPoint pointMin(999999, 999999);

		polyNodeFunc func = [&pointMax, &pointMin](ClipperLib::PolyNode* node) {
			for (ClipperLib::IntPoint& p : node->Contour)
			{
				if (pointMax.X < p.X)
					pointMax = p;
				if (pointMin.X > p.X)
				{
					pointMin = p;
				}
			}
		};
		mmesh::loopPolyTree(func, &poly);

		return pointMax.X - pointMin.X;
	}

	void NestedGenerator::buildOuter(bool onePoly)
	{
		ClipperLib::PolyTree treeTop, treeBottom;
		double hTop, hBottom;

		if (onePoly)
		{
			_buildBottom_onepoly(treeBottom, hBottom, 0);
			_buildTop_onepoly(treeTop, hTop, 0);
		}
		else
		{
			_buildTop(treeTop, hTop);
			_buildBottom(treeBottom, hBottom);
		}
		_buildFromSamePolyTree(&treeBottom, &treeTop);
	}

	void NestedGenerator::buildInner(bool onePoly)
	{
		float thickness = m_adParam.extend_width / 2.0 ;
		ClipperLib::PolyTree inner;

		ClipperLib::cInt len = getAABB(m_poly);
		len += m_adParam.bottom_extend_width * 10000;

		offsetPolyType(m_poly, -(thickness + m_adParam.top_extend_width), inner, m_adParam.bluntSharpCorners);

		polyNodeFunc func1 = [&len](ClipperLib::PolyNode* node) {
			for (ClipperLib::IntPoint& p : node->Contour)
			{
				p.X += len;
			}
		};
		mmesh::loopPolyTree(func1, &inner);

		std::vector<ClipperLib::PolyTree> middlePolys;
		middlePolys.resize(3);


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