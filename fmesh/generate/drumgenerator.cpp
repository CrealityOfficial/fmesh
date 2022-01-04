#include "drumgenerator.h"
#include "fmesh/contour/contour.h"
#include "clipperxyz/clipper.hpp"
#include "fmesh/clipper/circurlar.h"
#include "specialpoly.h"
#include <cmath>
#include "cmesh/mesh/roof.h"

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
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys);
		if (middlePolys.size())
			_buildTopBottomDiff(&middlePolys.front(), nullptr);
	}

	void DrumGenerator::buildShell()
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys,true);
		if (middlePolys.size())
			_buildTopBottom_onepoly(&middlePolys.front(), nullptr);
	}

	void DrumGenerator::buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree)
	{
		std::vector<ClipperLibXYZ::PolyTree> middlePolys;
		buildMiddle(middlePolys, true);
		//_buildBoardPoly(&bottomTree);
		if (middlePolys.size())
			offsetPolyType(middlePolys.front(), m_adParam.exoprtParam.bottom_offset, bottomTree, m_adParam.bluntSharpCorners);
	}

	void DrumGenerator::buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys, bool onePloy)
	{
		double thickness = m_adParam.extend_width / 2.0;

		float offsetH = 0.4;
		size_t drumHCount = 800;

		//float offsetr = 1.0 * m_adParam.shape_top_height / drumHCount;
		int lastvalueX = 0;
		int lastvalueY = 0;
		middlePolys.resize(1 + drumHCount);
		for (int i = 0; i < drumHCount; i++)
		{
			//float _offsetr = m_adParam.shape_top_height - sqrt(pow(m_adParam.shape_top_height, 2) - pow(i * offsetr, 2));
			float _offsetr = 0.05f;
			if (onePloy)
			{
				offsetPolyTreeMiter(m_poly, -_offsetr * i, middlePolys.at(i));
				//setPolyTreeZ(middlePolys.at(i), delta);
				//_simplifyPoly(&middlePolys.at(i));
			}
			else
			{
				offsetAndExtendPolyTreeMiter(m_poly, -0.05*i, thickness, middlePolys.at(i));
				//_simplifyPoly(&middlePolys.at(i));
			}
			if (i>1)
			{
				ClipperLibXYZ::IntPoint P = getAABBvalue(&middlePolys.at(i));
				ClipperLibXYZ::IntPoint P1 = getAABBvalue(&middlePolys.at(i - 1));
				int _lastvalueX = std::abs(P.X - P1.X);
				int _lastvalueY = std::abs(P.Y - P1.Y);
				if (P.X < P1.X *2/3 || P.Y < P1.Y * 2 / 3)
				{
					middlePolys.at(i).Clear();
					middlePolys.at(i-1).Clear();
					break;
				}				
				lastvalueX = _lastvalueX;
				lastvalueY = _lastvalueX;
			}

			if (i > 0 && GetPolyCount(&middlePolys.at(i)) != GetPolyCount(&middlePolys.at(i - 1)))
			{
				middlePolys.at(i).Clear();
				break;
			}

		}

		while (middlePolys.size() && !middlePolys.back().ChildCount())
		{
			middlePolys.pop_back();
		}

		if (middlePolys.size())
		{
			middlePolys.pop_back();
		}
		else
		{
			return;
		}

		//float delta1 = 0.0f;
		//float delta2 = 0.0f;
		//double bottomHeight = m_adParam.total_height - (middlePolys.size() - 1) * offsetH;
		//if (bottomHeight < 0)
		//	bottomHeight = 0;

		float offsetr1 = m_adParam.shape_top_height / drumHCount;
		//double bottomHeight = m_adParam.total_height - m_adParam.shape_top_height;
		//float raduis = m_adParam.shape_top_height;
		
		float raduis = middlePolys.size() > 100? middlePolys.size()/10 : raduis;
		double bottomHeight =  m_adParam.total_height - raduis >0? m_adParam.total_height - raduis :0.0f;
		float delta2 = 0.0f;
		for (size_t i = 0; i < middlePolys.size(); i++)
		{
			float _offsetr = sqrt(pow(raduis, 2) - pow((i * offsetr1 - raduis), 2));
			delta2 = bottomHeight + _offsetr;
			setPolyTreeZ(middlePolys.at(i), delta2);
		}

		for (size_t i = 0; i < middlePolys.size() - 1; i++)
		{
			//delta1 = bottomHeight + offsetH * i;
			//delta2 = bottomHeight + offsetH * (i + 1);
			//if (i % 2 || !i)
			//	setPolyTreeZ(middlePolys.at(i), delta1);
			//setPolyTreeZ(middlePolys.at(i + 1), delta2);

			//if (onePloy)
			//	_buildFromDiffPolyTree_onePoly(&middlePolys.at(i), &middlePolys.at(i + 1));//outer
			//else
			_buildFromDiffPolyTree_diffSafty(&middlePolys.at(i), &middlePolys.at(i + 1));
		}
		std::vector<Patch*> patches;
		if (onePloy)
		{
			//_simplifyPoly(&middlePolys.back(), 50);
			//skeletonPolyTree(middlePolys.back(), delta2, patches, middlePolys.size() / 150, onePloy);
		}
		else
		{
			_simplifyPoly(&middlePolys.back());
			//skeletonPolyTree(middlePolys.back(), delta2, patches, middlePolys.size() / 300);
			skeletonPolyTree(middlePolys.back(), delta2, patches, 0.1);
		}
		addPatches(patches);
	}

/*
	void DrumGenerator::buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys, bool onePloy)
	{
		ClipperLibXYZ::Path* path = new ClipperLibXYZ::Path;
		ClipperLibXYZ::PolyTree pskeleton;
		copy2PolyTree(m_poly, pskeleton);
		mmesh::skeletonPoints(&pskeleton, path);
		ClipperLibXYZ::Paths paths;
		sortPath(path, &paths,true);
		//origin
		ClipperLibXYZ::Paths pathOrigin;
		ClipperLibXYZ::PolyTreeToPaths(m_poly, pathOrigin);	
		float len = optimizePaths(paths, pathOrigin)/1000.00f;

		float thickness = m_adParam.extend_width / 2.0;

		ClipperLibXYZ::PolyTree poly;
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
void DrumGenerator::buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys, bool onePloy)
{
	ClipperLibXYZ::PolyTree skeletonpoly;
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