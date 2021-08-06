#include "drumgenerator.h"
#include "fmesh/contour/contour.h"
#include <clipper/clipper.hpp>
#include "mmesh/clipper/circurlar.h"
#include "specialpoly.h"
#include "mmesh/cgal/roof.h"

namespace fmesh
{
	DrumGenerator::DrumGenerator()
	{

	}

	DrumGenerator::~DrumGenerator()
	{

	}

	void DrumGenerator::build()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys);
		_buildTopBottomDiff(&middlePolys.front(), nullptr);
	}

	void DrumGenerator::buildShell()
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys,true);
		_buildTopBottom_onepoly(&middlePolys.front(), nullptr);
	}

	void DrumGenerator::buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		std::vector<ClipperLib::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		copy2PolyTree(middlePolys.front(), bottomTree);
		//_buildBoardPoly(&bottomTree);
	}

	void DrumGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePloy)
	{
		double thickness = m_adParam.extend_width / 2.0;

		double x = dmax.x - dmin.x;
		double y = dmax.y - dmin.y;

		float offsetH = 0.4;
		size_t drumHCount = 150;

		float offsetr = 1.0 * m_adParam.shape_top_height / drumHCount;
		middlePolys.resize(1 + drumHCount);
		for (size_t i = 0; i < drumHCount; i++)
		{
			float _offsetr = m_adParam.shape_top_height - sqrt(pow(m_adParam.shape_top_height, 2) - pow(i * offsetr, 2));

			if (onePloy)
			{
				offsetPolyTreeMiter(m_poly, -_offsetr * 50, middlePolys.at(i));
				//setPolyTreeZ(middlePolys.at(i), delta);
				//_simplifyPoly(&middlePolys.at(i));
			}
			else
			{
				offsetAndExtendPolyTreeMiter(m_poly, -_offsetr * 50, thickness, middlePolys.at(i));
				//_simplifyPoly(&middlePolys.at(i));
			}
			if (i && GetPolyCount(&middlePolys.at(i)) != GetPolyCount(&middlePolys.at(i - 1)))
			{
				middlePolys.at(i).Clear();
				break;
			}
		}

		while (!middlePolys.back().ChildCount())
		{
			middlePolys.pop_back();
		}

		if (middlePolys.size())
		{
			middlePolys.pop_back();
		}

		float delta1 = 0.0f;
		float delta2 = 0.0f;
		double bottomHeight = m_adParam.total_height - (middlePolys.size() - 1) * offsetH;
		if (bottomHeight < 0)
			bottomHeight = 0;

		for (size_t i = 0; i < middlePolys.size() - 1; i++)
		{
			delta1 = bottomHeight + offsetH * i;
			delta2 = bottomHeight + offsetH * (i + 1);
			if (i % 2 || !i)
				setPolyTreeZ(middlePolys.at(i), delta1);
			setPolyTreeZ(middlePolys.at(i + 1), delta2);

			//if (onePloy)
			//	_buildFromDiffPolyTree_onePoly(&middlePolys.at(i), &middlePolys.at(i + 1));//outer
			//else
			_buildFromDiffPolyTree_diffSafty(&middlePolys.at(i), &middlePolys.at(i + 1));
		}
		std::vector<Patch*> patches;
		if (onePloy)
		{
			_simplifyPoly(&middlePolys.back(), 50);
			skeletonPolyTree(middlePolys.back(), delta2, patches, middlePolys.size() / 8, onePloy);
		}
		else
		{
			_simplifyPoly(&middlePolys.back(), 40);
			skeletonPolyTree(middlePolys.back(), delta2, patches, middlePolys.size() / 100);
		}
		addPatches(patches);
	}

	/*
	void DrumGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePloy)
	{
		ClipperLib::Path* path = new ClipperLib::Path;
		ClipperLib::PolyTree pskeleton;
		copy2PolyTree(m_poly, pskeleton);
		mmesh::skeletonPoints(&pskeleton, path);
		ClipperLib::Paths paths;
		sortPath(path, &paths,true);
		//origin
		ClipperLib::Paths pathOrigin;
		ClipperLib::PolyTreeToPaths(m_poly, pathOrigin);	
		float len = optimizePaths(paths, pathOrigin)/1000.00f;

		float thickness = m_adParam.extend_width / 2.0;

		ClipperLib::PolyTree poly;
		m_poly.Clear();
		extendPolyTreeOpen(paths, (thickness * 2 + len), m_poly);

		size_t drumHCount = 20;
		double offset = 1.0 * (len + thickness) / (drumHCount + 2);
		middlePolys.resize(drumHCount);
		for (size_t i = 0; i < drumHCount; i++)
		{
			offsetAndExtendPolyTreeMiter(m_poly, -offset * i, thickness, middlePolys.at(i));
		}

		float offsetr = m_adParam.shape_top_height / drumHCount;
		double bottomHeight = m_adParam.total_height - m_adParam.shape_top_height;
		float delta2 = 0.0f;
		for (size_t i = 0; i < drumHCount; i++)
		{
			float _offsetr = sqrt(pow(m_adParam.shape_top_height, 2) - pow((i * offsetr - m_adParam.shape_top_height), 2));
			delta2 = bottomHeight + _offsetr;
			setPolyTreeZ(middlePolys.at(i), delta2);
		}

		for (size_t i = 0; i < drumHCount - 1; i++)
		{
			_buildFromDiffPolyTree_diffSafty(&middlePolys.at(i), &middlePolys.at(i + 1));
		}
		std::vector<Patch*> patches;
		if (onePloy)
		{
			_simplifyPoly(&middlePolys.back(), 50);
			skeletonPolyTree(middlePolys.back(), delta2, patches, middlePolys.size() / 8, onePloy);
		}
		else
		{
			_simplifyPoly(&middlePolys.back(), 18);
			skeletonPolyTree(middlePolys.back(), delta2, patches, 0);
		}
		addPatches(patches);
	}
	*/

/*
void DrumGenerator::buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePloy)
{
	ClipperLib::PolyTree skeletonpoly;
	//offsetPolyTreeMiter(m_poly, -1, _skeletonpoly);
	double thickness = m_adParam.extend_width / 2.0;
	double len = skeletonPoly(m_poly, skeletonpoly, thickness);
	float offsetH = 0.4;

	size_t drumHCount = 20;
	double offset = 1.0 * (len + thickness) / (drumHCount + 2);
	middlePolys.resize(drumHCount);
	for (size_t i = 0; i < drumHCount; i++)
	{
		offsetAndExtendPolyTreeMiter(skeletonpoly, -offset * i, thickness, middlePolys.at(i));
	}

	//double bottomHeight = m_adParam.total_height - (middlePolys.size() - 1) * offsetH;
	//if (bottomHeight < 0)
	//	bottomHeight = 0;

	//float offsetr = 1.0 * m_adParam.shape_top_height / drumHCount;
	float offsetr = m_adParam.shape_top_height / drumHCount;
	double bottomHeight = m_adParam.total_height - m_adParam.shape_top_height;
	float delta2 = 0.0f;
	for (size_t i = 0; i < drumHCount; i++)
	{
		float _offsetr = sqrt(pow(m_adParam.shape_top_height, 2) - pow((i * offsetr - m_adParam.shape_top_height), 2));
		delta2 = bottomHeight + _offsetr;
		setPolyTreeZ(middlePolys.at(i), delta2);
	}


	for (size_t i = 0; i < drumHCount - 1; i++)
	{
		_buildFromDiffPolyTree_diffSafty(&middlePolys.at(i), &middlePolys.at(i + 1));
	}
	std::vector<Patch*> patches;
	if (onePloy)
	{
		_simplifyPoly(&middlePolys.back(), 50);
		skeletonPolyTree(middlePolys.back(), delta2, patches, middlePolys.size() / 8, onePloy);
	}
	else
	{
		_simplifyPoly(&middlePolys.back(), 18);
		skeletonPolyTree(middlePolys.back(), delta2, patches, 0);
	}
	addPatches(patches);
}
	

	void DrumGenerator::initTestData()
	{

	}
*/
}