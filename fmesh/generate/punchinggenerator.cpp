#include "punchinggenerator.h"
#include "fmesh/clipper/circurlar.h"
#include "specialpoly.h"

namespace fmesh
{
	PunchingGenerator::PunchingGenerator()
	{

	}

	PunchingGenerator::~PunchingGenerator()
	{

	}

	void PunchingGenerator::build()
	{
		m_adParam.bottom_type = ADBottomType::adbt_none;

		double middleoffset = 0;
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset);
		if (middlePolys.size() == 0)
			return;
		_buildTopBottomDiff(nullptr, &middlePolys.back(), 0, 0);
	}

	void PunchingGenerator::buildShell()
	{
		m_adParam.bottom_type = ADBottomType::adbt_none;

		double middleoffset = 0;
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset,true);
		if (middlePolys.size() == 0)
			return;
		_buildTopBottom_onepoly(nullptr, &middlePolys.back(), 0, 0);
	}

	void PunchingGenerator::buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree)
	{
		double middleoffset = 0;
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, middleoffset, true);
		if (middlePolys.size() == 0)
			return;
		//copy2PolyTree(middlePolys.back(), topTree);
		offsetPolyType(middlePolys.front(), m_adParam.exoprtParam.bottom_offset, bottomTree, m_adParam.bluntSharpCorners);
	}

	void PunchingGenerator::buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys, double& middleoffset, bool onePoly)
	{
		//initTestData();
		float thickness = m_adParam.extend_width / 2.0;
		float offset = 0;
		float totalH = m_adParam.total_height;

		if (m_adParam.bottom_extend_width <2.0f)
		{
			m_adParam.bottom_extend_width = 6.0f;
		}

		float bootomH = m_adParam.bottom_layers * 0.3;

		if (onePoly)
			middlePolys.resize(4);
		else
			middlePolys.resize(3);
		
		ClipperLibXYZ::PolyTree  middlePath;

		offsetAndExtendpolyType(m_poly, offset, thickness, 0, middlePolys.at(0), m_adParam.bluntSharpCorners);
		//offsetAndExtendpolyType(m_poly, offset/2.0f, thickness, 0, middlePath, m_adParam.bluntSharpCorners);

		offsetAndExtendpolyType(m_poly, offset, thickness, bootomH, middlePolys.at(1), m_adParam.bluntSharpCorners);
		offsetAndExtendpolyType(m_poly, offset, thickness, bootomH, middlePolys.at(2), m_adParam.bluntSharpCorners);
		offsetExteriorInner(middlePolys.at(0), m_adParam.bottom_extend_width, 0.0);
		offsetExteriorInner(middlePolys.at(1), m_adParam.bottom_extend_width, bootomH);
		if (onePoly)
			offsetAndExtendpolyType(m_poly, offset, thickness, totalH, middlePolys.at(3), m_adParam.bluntSharpCorners);
		//_buildFromSamePolyTree(&middlePolys.at(0), &middlePolys.at(1));
		//_fillPolyTree(&middlePolys.at(0), true);

		//ClipperLibXYZ::PolyTree out;
		//fmesh::xor2PolyTrees(&middlePolys.at(1), &middlePolys.at(2), out);
		//_buildFromDiffPolyTree_firstLayer(&out);

		ClipperLibXYZ::PolyTree  newPath;
		ClipperLibXYZ::Paths paths;

		fmesh::offsetPolyTree(m_poly, -(m_adParam.bottom_extend_width/2.0f + thickness*3/2.0), middlePath);
		polyNodeFunc func = [](ClipperLibXYZ::PolyNode* node) {
			ClipperLibXYZ::CleanPolygon(node->Contour, 5);
		};
		fmesh::loopPolyTree(func, &middlePath);

		ClipperLibXYZ::Paths sourcePaths;
		ClipperLibXYZ::PolyTreeToPaths(middlePath, sourcePaths);
		fmesh::generateLines(sourcePaths, paths, m_adParam.roundGap, m_adParam.roundRadius);
		ClipperLibXYZ::Paths newPaths;
		fmesh::generateRounds(paths, newPaths, m_adParam.roundGap, m_adParam.roundRadius);

		ClipperLibXYZ::Clipper clipper;
		polyNodeFunc func1 = [&func, &clipper](ClipperLibXYZ::PolyNode* node) {
				clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
		};
		fmesh::loopPolyTree(func1, &middlePolys.at(0));
		for (ClipperLibXYZ::Path& path : newPaths)
		{
			ClipperLibXYZ::ReversePath(path);
			clipper.AddPath(path, ClipperLibXYZ::ptClip, true);
		}
		ClipperLibXYZ::PolyTree newPath2;
		ClipperLibXYZ::PolyTree newPath3;
		clipper.Execute(ClipperLibXYZ::ctUnion, newPath2, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);
		clipper.Execute(ClipperLibXYZ::ctUnion, newPath3, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);
		if(!onePoly)
			_fillPolyTree(&newPath2, true);

		//ClipperLibXYZ::PolyTree out;
		middlePolys.at(0).Clear();
		setPolyTreeZ(newPath2, bootomH);
		if (!onePoly)
		{
			fmesh::xor2PolyTrees(&newPath2, &middlePolys.at(2), middlePolys.at(0));
			_buildFromDiffPolyTree_firstLayer(&middlePolys.at(0));
		}

		_buildFromSamePolyTree(&newPath3, &newPath2);

		if (onePoly)
			_buildFromDiffPolyTree_diffSafty(&middlePolys.at(2), &middlePolys.at(3),1,3);
	}

	void PunchingGenerator::initTestData()
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