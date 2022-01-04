#include "lampgenerator.h"
#include "fmesh/clipper/circurlar.h"
#include "specialpoly.h"
#include "cmesh/mesh/roof.h"

namespace fmesh
{
	LampGenerator::LampGenerator()
	{

	}

	LampGenerator::~LampGenerator()
	{

	}

	void LampGenerator::build()
	{

		//m_adParam.bottom_type = ADBottomType::adbt_none;

		double middleoffset = 0;
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset);
		if (middlePolys.size() == 0)
			return;
		_buildTopBottomDiff(nullptr,&middlePolys.back(), 0, 0);
	}

	void LampGenerator::buildShell()
	{
		m_adParam.bottom_type = ADBottomType::adbt_none;

		double middleoffset = 0;
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset, true);
		if (middlePolys.size() == 0)
			return;
		_buildTopBottom_onepoly(nullptr, &middlePolys.back(), 0, 0);
	}

	void LampGenerator::buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree)
	{
		double middleoffset = 0;
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset, true);
		if (middlePolys.size() == 0)
			return;
		//copy2PolyTree(middlePolys.back(), topTree);
		offsetPolyType(middlePolys.front(), m_adParam.exoprtParam.bottom_offset, bottomTree, m_adParam.bluntSharpCorners);
	}

	void LampGenerator::buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys, double& middleoffset, bool onePoly)
	{
		//initTestData();
		float thickness = m_adParam.extend_width / 2.0;
		float offset = 0;
		float totalH = m_adParam.total_height;
		float thicknessH = thickness*2.0;

		middlePolys.resize(3);
		offsetAndExtendpolyType(m_poly, offset, thickness, 0, middlePolys.at(0), m_adParam.bluntSharpCorners);
		offsetAndExtendpolyType(m_poly, offset, thickness, thicknessH, middlePolys.at(1), m_adParam.bluntSharpCorners);

		if (onePoly)
		{
			offsetPolyTree(m_poly, thickness / 2, middlePolys.at(2));
			setPolyTreeZ(middlePolys.at(2), thicknessH);
			//offsetAndExtendpolyType(m_poly, offset, thickness, thicknessH, middlePolys.at(2), m_adParam.bluntSharpCorners);
		}
		else
			offsetAndExtendpolyType(m_poly, offset, thickness, thicknessH, middlePolys.at(2), m_adParam.bluntSharpCorners);

		ClipperLibXYZ::Path* path = new ClipperLibXYZ::Path;
		cmesh::skeletonPoints(&m_poly, path);

		ClipperLibXYZ::Paths paths;
		sortPath(path, &paths);

		ClipperLibXYZ::Paths inPaths;
		ClipperLibXYZ::Paths outPaths;
		generateLines(paths, inPaths, m_adParam.roundGap, m_adParam.roundRadius, true);
		generateRounds(inPaths, outPaths, m_adParam.roundGap, m_adParam.roundRadius);

		ClipperLibXYZ::Clipper clipper;
		int flag = 0;
		//bottom
		polyNodeFunc func1 = [&func1, &clipper, &flag](ClipperLibXYZ::PolyNode* node) {
			if (checkFlag(node, flag))
			{
				if (2 == flag)
				{
					ClipperLibXYZ::ReversePath(node->Contour);
					clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
				}
				else
					clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
			}
				
		};
		flag = 3;//outer
		fmesh::loopPolyTree(func1, &middlePolys.at(0));
		for (ClipperLibXYZ::Path& path : outPaths)
		{
			ClipperLibXYZ::ReversePath(path);
			clipper.AddPath(path, ClipperLibXYZ::ptClip, true);
		}
		ClipperLibXYZ::PolyTree newPolyB;
		clipper.Execute(ClipperLibXYZ::ctUnion, newPolyB, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);

		//top
		clipper.Clear();
		flag = 2;//inner
		fmesh::loopPolyTree(func1, &middlePolys.at(1));
		for (ClipperLibXYZ::Path& path : outPaths)
		{
			clipper.AddPath(path, ClipperLibXYZ::ptClip, true);
		}
		ClipperLibXYZ::PolyTree newPolyT;
		clipper.Execute(ClipperLibXYZ::ctUnion, newPolyT, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);
		setPolyTreeZ(newPolyT, thicknessH);
		
		_buildFromSamePolyTree(&middlePolys.at(0), &middlePolys.at(1), 3);
		_buildFromSamePolyTree(&newPolyB, &newPolyT,2);

		_fillPolyTree(&newPolyT);
		_fillPolyTree(&newPolyB, true);

		middlePolys.at(0).Clear();
		copy2PolyTree(newPolyT, middlePolys.at(0));
}

	void LampGenerator::initTestData()
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