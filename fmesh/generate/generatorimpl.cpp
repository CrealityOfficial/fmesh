#include "generatorimpl.h"
#include "fmesh/contour/polytree.h"
#include "fmesh/generate/trimeshbuilder.h"
#include "fmesh/contour/path.h"

#include "fmesh/clipper/circurlar.h"

namespace fmesh
{
	GeneratorImpl::GeneratorImpl()
		:m_paths(nullptr)
		, m_topTree(nullptr)
		, m_bottomTree(nullptr)
		, m_tracer(nullptr)
	{

	}

	GeneratorImpl::~GeneratorImpl()
	{
	}

	trimesh::TriMesh* GeneratorImpl::generate(ClipperLibXYZ::Paths* paths, const ADParam& param)
	{
		m_paths = paths;
		m_adParam = param;

		ClipperLibXYZ::IntPoint bmin;
		ClipperLibXYZ::IntPoint bmax;
		fmesh::calculatePathBox(paths, bmin, bmax);
		//scale
		trimesh::vec3 bMin = trimesh::vec3(INT2MM(bmin.X), INT2MM(bmin.Y), INT2MM(bmin.Z));
		trimesh::vec3 bMax = trimesh::vec3(INT2MM(bmax.X), INT2MM(bmax.Y), INT2MM(bmax.Z));
		dmin = trimesh::vec2(bMin);
		dmax = trimesh::vec2(bMax);

		fmesh::convertPaths2PolyTree(m_paths, m_poly);
		if (!paths || paths->size() == 0)
			return nullptr;

		build();
		//buildShell();

		trimesh::TriMesh* mesh = generateFromPatches();
		releaseResources();
		return mesh;
	}

	trimesh::TriMesh* GeneratorImpl::generateShell(ClipperLibXYZ::Paths* paths, const ADParam& param,
		ExportParam* exportParam, ClipperLibXYZ::PolyTree* topTree, ClipperLibXYZ::PolyTree* bottomTree)
	{
		m_paths = paths;
		m_adParam = param;

		if (exportParam)
			m_exportParam = *exportParam;
		m_topTree = topTree;
		m_bottomTree = bottomTree;

		ClipperLibXYZ::IntPoint bmin;
		ClipperLibXYZ::IntPoint bmax;
		fmesh::calculatePathBox(paths, bmin, bmax);
		//scale
		trimesh::vec3 bMin = trimesh::vec3(INT2MM(bmin.X), INT2MM(bmin.Y), INT2MM(bmin.Z));
		trimesh::vec3 bMax = trimesh::vec3(INT2MM(bmax.X), INT2MM(bmax.Y), INT2MM(bmax.Z));
		dmin = trimesh::vec2(bMin);
		dmax = trimesh::vec2(bMax);

		fmesh::convertPaths2PolyTree(m_paths, m_poly);
		if (!paths || paths->size() == 0)
			return nullptr;

		buildShell();

		trimesh::TriMesh* mesh = generateFromPatches();
		releaseResources();
		return mesh;
	}

	void GeneratorImpl::setup(const ADParam& param, ClipperLibXYZ::Paths* paths, mmesh::StatusTracer* tracer)
	{
		m_tracer = tracer;

		m_paths = paths;
		m_adParam = param;

		ClipperLibXYZ::IntPoint bmin;
		ClipperLibXYZ::IntPoint bmax;
		fmesh::calculatePathBox(paths, bmin, bmax);
		//scale
		trimesh::vec3 bMin = trimesh::vec3(INT2MM(bmin.X), INT2MM(bmin.Y), INT2MM(bmin.Z));
		trimesh::vec3 bMax = trimesh::vec3(INT2MM(bmax.X), INT2MM(bmax.Y), INT2MM(bmax.Z));
		dmin = trimesh::vec2(bMin);
		dmax = trimesh::vec2(bMax);

		if (m_paths)
			fmesh::convertPaths2PolyTree(m_paths, m_poly);
	}

	trimesh::TriMesh* GeneratorImpl::generate()
	{
		build();

		trimesh::TriMesh* mesh = nullptr;
		if(!m_tracer || !m_tracer->interrupt())
			mesh = generateFromPatches();
		releaseResources();
		return mesh;
	}

	trimesh::TriMesh* GeneratorImpl::generateShell()
	{
		buildShell();

		trimesh::TriMesh* mesh = nullptr;
		if (!m_tracer || !m_tracer->interrupt())
			mesh = generateFromPatches();
		releaseResources();
		return mesh;
	}

	void GeneratorImpl::generateBoard(const ExportParam& param, ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree)
	{
		m_exportParam = param;
		topTree.Clear();
		bottomTree.Clear();
		buildBoard(topTree, bottomTree);
	}

	trimesh::TriMesh* GeneratorImpl::generate(ClipperLibXYZ::PolyTree* tree, const ADParam& param,
		ExportParam* exportParam, ClipperLibXYZ::PolyTree* topTree, ClipperLibXYZ::PolyTree* bottomTree)
	{
		return nullptr;
		//m_adParam = param;
		//
		//if (exportParam)
		//	m_exportParam = *exportParam;
		//m_topTree = topTree;
		//m_bottomTree = bottomTree;
		//
		//m_poly = tree;
		//ClipperLibXYZ::IntPoint bmin;
		//ClipperLibXYZ::IntPoint bmax;
		//fmesh::calculatePathBox(paths, bmin, bmax);
		////scale
		//trimesh::vec3 bMin = trimesh::vec3(INT2MM(bmin.X), INT2MM(bmin.Y), INT2MM(bmin.Z));
		//trimesh::vec3 bMax = trimesh::vec3(INT2MM(bmax.X), INT2MM(bmax.Y), INT2MM(bmax.Z));
		//dmin = trimesh::vec2(bMin);
		//dmax = trimesh::vec2(bMax);
		//
		//fmesh::convertPaths2PolyTree(m_paths, m_poly);
		//if (!paths || paths->size() == 0)
		//	return nullptr;
		//
		//build();
		//
		//trimesh::TriMesh* mesh = generateFromPatches();
		//releaseResources();
		//return mesh;
	}

	void GeneratorImpl::addPatch(Patch* patch, bool invert)
	{
		if (patch)
		{
			if (invert)
			{
				size_t size = patch->size() / 3;
				for (size_t i = 0; i < size; ++i)
					std::swap(patch->at(3 * i + 1), patch->at(3 * i + 2));
			}

			m_patches.push_back(patch);
		}
	}

	void GeneratorImpl::addPatches(std::vector<Patch*>& patches, bool invert)
	{
		for (Patch* patch : patches)
			addPatch(patch, invert);
	}

	trimesh::TriMesh* GeneratorImpl::generateFromPatches()
	{
		return buildFromPatches(m_patches);
	}

	void GeneratorImpl::releaseResources()
	{
		for (Patch* patch : m_patches)
			delete patch;
		m_patches.clear();
	}

	void GeneratorImpl::_fillPolyTree(ClipperLibXYZ::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;
		fillComplexPolyTree(tree, patches);

		addPatches(patches, invert);
	}

	void GeneratorImpl::_fillPolyTreeDepth14(ClipperLibXYZ::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;

		fillPolyTreeDepth14(tree, patches);
		addPatches(patches, invert);
	}

	void GeneratorImpl::_fillPolyTreeOnePloy(ClipperLibXYZ::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;

		fillPolyTreeDepthOnePoly(tree, patches);
		addPatches(patches, invert);
	}

	void GeneratorImpl::_fillPolyTreeDepth23(ClipperLibXYZ::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;

		fillPolyTreeDepth23(tree, patches);
		addPatches(patches, invert);
	}

	void GeneratorImpl::_fillPolyTreeInner(ClipperLibXYZ::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;
		fillPolyNodeInner(tree, patches);

		addPatches(patches, invert);
	}

	void GeneratorImpl::_buildFromSamePolyTree(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, int flag)
	{
		addPatch(buildFromSamePolyTree(treeLower, treeUp, flag));
	}

	void GeneratorImpl::_buildFromDiffPolyTree(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, int flag)
	{
		std::vector<Patch*> patches;
		buildFromDiffPolyTreeSafty(treeLower, treeUp, patches, 1.0, flag);
		addPatches(patches);
	}

	void GeneratorImpl::_buildFromDiffPolyTree_diffSafty(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta, int flag, bool invert, bool invertXor)
	{
		ClipperLibXYZ::PolyTree out;

		std::vector<Patch*> patches;
		buildFromDiffPolyTree_SameAndDiffSafty(treeLower, treeUp, patches, flag, out, delta);
		if (patches.size())
			addPatches(patches,invert);
		if (out.ChildCount() > 0)
		{
			//_fillPolyTreeReverse(&inner,false);
			//_fillPolyTreeReverse(&out,true);
			fillComplexPolyTreeReverseInner(&out, patches);
			addPatches(patches);
		}
	}

	void GeneratorImpl::_buildFromDiffPolyTree_diff(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta, int flag, bool invert)
	{
		ClipperLibXYZ::PolyTree out;
		std::vector<Patch*> patches;

		xor2PolyTrees(treeLower, treeUp, out);
		if (out.ChildCount() > 0)
		{
			fillComplexPolyTreeReverseInner(&out, patches);
			//_fillPolyTreeReverse(&out, true);
			addPatches(patches);
		}
	}

	void GeneratorImpl::_buildFromDiffPolyTree_all(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta, int flag,bool invertInner, bool invertOuter)
	{
		ClipperLibXYZ::PolyTree treeLowerInner;
		ClipperLibXYZ::PolyTree treeUpInner;
		;//2: Inner
		if (GetPolyCount(treeLower, 2) == GetPolyCount(treeUp, 2))
		{
			_buileDetachPloyTree(treeLower, treeLowerInner, delta,2);
			_buileDetachPloyTree(treeUp, treeUpInner, delta,2);
			_buildFromDiffPolyTree_all_same(&treeLowerInner, &treeUpInner, delta, flag, false);
		}
		else
		{
			ClipperLibXYZ::PolyTree outInner;
			fmesh::xor2PolyTrees(treeLower, treeUp, outInner, 2);//inner
			_fillPolyTreeReverseInnerNew(&outInner, invertInner);
		}

		//3:outer
		if (GetPolyCount(treeLower, 3) == GetPolyCount(treeUp, 3))
		{
			ClipperLibXYZ::PolyTree treeLowerOuter;
			ClipperLibXYZ::PolyTree treeUpOuter;

			_buileDetachPloyTree(treeLower, treeLowerOuter, delta,3);
			_buileDetachPloyTree(treeUp, treeUpOuter, delta,3);
			_buildFromDiffPolyTree_all_same(&treeLowerOuter, &treeUpOuter, delta, flag, false);
		}
		else
		{
			ClipperLibXYZ::PolyTree outOuter;
			fmesh::xor2PolyTrees(treeLower, treeUp, outOuter, 3);
			_fillPolyTreeReverseInnerNew(&outOuter, invertOuter);
		}

		//if (GetPolyCount(treeLower) == GetPolyCount(treeUp))
		//	_buildFromDiffPolyTree_all_same(treeLower, treeUp, delta, flag, false);
		//else
		//	_buildFromDiffPolyTree_all_diff(treeUp, treeLower, flag);
	}

	void GeneratorImpl::_buildFromDiffPolyTree_all_same(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta, int flag, bool invert)
	{
		std::vector<Patch*> patches;
		buildFromDiffPolyTreeSafty(treeLower, treeUp, patches, delta, flag);
		addPatches(patches, invert);
	}

	void GeneratorImpl::_buildFromDiffPolyTree_xor(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta, int flag, bool invert)
	{
		ClipperLibXYZ::PolyTree out;
		//fmesh::xor2PolyTrees(treeUp, treeLower, out, flag);
		std::vector<Patch*> patches;
		xor2PolyTrees(treeUp, treeLower, out, flag);
		if (out.ChildCount() > 0)
		{
			//savePolyTree(&out, "buildFromXOR");
			_fillPolyTreeSameZ(&out, invert);
		}
	}

	void GeneratorImpl::_buildFromDiffPolyTree_Inner(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta /*= 1.0*/, int flag /*= 0*/, bool invert /*= false*/)
	{
		ClipperLibXYZ::PolyTree out;
		//fmesh::xor2PolyTrees(treeUp, treeLower, out, flag);
		std::vector<Patch*> patches;
		buildFromDiffPolyTree_SameAndDiffSafty(treeLower, treeUp, patches, flag, out, delta);
		if (patches.size())
			addPatches(patches);
		if (out.ChildCount() > 0)
		{
			_fillPolyTreeReverseInner(&out, invert);
		}
	}

	void GeneratorImpl::_buildFromDiffPolyTree_onePoly(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta /*= 1.0*/, int flag /*= 0*/, bool invert /*= false*/)
	{
		ClipperLibXYZ::PolyTree out;
		//fmesh::xor2PolyTrees(treeUp, treeLower, out, flag);
		std::vector<Patch*> patches;
		buildFromDiffPolyTree_SameAndDiffSafty(treeLower, treeUp, patches, flag, out, delta);
		if (patches.size())
			addPatches(patches);
		if (out.ChildCount() > 0)
		{
			std::vector<Patch*> patches;
			fillComplexPolyTree_onePloy(&out, patches, invert);
			addPatches(patches);
		}
	}

	void GeneratorImpl::_fillPolyTreeReverseInner(ClipperLibXYZ::PolyTree* tree, bool invert /*= false*/)
	{
		std::vector<Patch*> patches;

		//test
		//fillComplexPolyTreeReverseInnerNew(tree, patches, invert);

		fillComplexPolyTreeReverseInner(tree, patches, invert);
		//fillComplexPolyTree(tree, patches);

		addPatches(patches);
	}

	void GeneratorImpl::_fillPolyTreeReverse(ClipperLibXYZ::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;
		fillComplexPolyTreeReverse(tree, patches, invert);
		addPatches(patches);
	}

	void GeneratorImpl::_fillPolyTreeReverseInnerNew(ClipperLibXYZ::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;

		//test
		fillComplexPolyTreeReverseInnerNew(tree, patches, invert);
		//fillComplexPolyTree(tree, patches);

		addPatches(patches);
	}

	void GeneratorImpl::_fillPolyTreeSameZ(ClipperLibXYZ::PolyTree* tree, bool invert /*= false*/)
	{
		std::vector<Patch*> patches;
		//fillComplexPolyTreeReverseInner(tree, patches);
		fillComplexPolyTree(tree, patches);

		addPatches(patches, invert);
	}

	void GeneratorImpl::_dealPolyTreeAxisZ(ClipperLibXYZ::PolyTree* tree, double slope, double min, double height)
	{
		dealPolyTreeAxisZ(tree, slope, min, height);
	}

	void GeneratorImpl::_buildFromDiffPolyTree_firstLayer(ClipperLibXYZ::PolyTree* treeLower, int flag /*= 0*/)
	{
		std::vector<Patch*> patches;
		fillComplexPolyTree(treeLower, patches);
		addPatches(patches, flag);
	}

	void GeneratorImpl::_buileMergePloyTree(ClipperLibXYZ::PolyTree* treeOuter, ClipperLibXYZ::PolyTree* treeInner, ClipperLibXYZ::PolyTree& treeNew)
	{
		ClipperLibXYZ::ClipperOffset offset;
		bool bReverse = false;
		polyNodeFunc func = [&func, &offset, &bReverse](ClipperLibXYZ::PolyNode* node) {
			if (bReverse)
				;// ClipperLibXYZ::ReversePath(node->Contour);
			offset.AddPath(node->Contour, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedPolygon);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};
		func(treeOuter);
		bReverse = true;
		func(treeInner);
		offset.Execute(treeNew, 0);
	}

	void GeneratorImpl::_buileDetachPloyTree(ClipperLibXYZ::PolyTree* tree, ClipperLibXYZ::PolyTree& treeNew,double delta,int flag)//0 all 2 inner 3 outer
	{
		ClipperLibXYZ::ClipperOffset offset;
		double z=0;
		int _flag = flag;
		polyNodeFunc func = [&func, &offset, &_flag, &z](ClipperLibXYZ::PolyNode* node) {
			int depth = testPolyNodeDepth(node);
			if ((_flag == 2 && (depth == 2 || depth == 3 || depth == 6 || depth == 7))
				|| (_flag == 3 && (depth == 1 || depth == 4 || depth == 5 || depth == 8))
				|| _flag == 0)
			{
				offset.AddPath(node->Contour, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedPolygon);
				if (!z && node->Contour.size())
				{
					z = node->Contour.at(0).Z;
				}
			}
			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};
		func(tree);
		offset.Execute(treeNew, 0);

		polyNodeFunc func1 = [&func1, &z,&flag](ClipperLibXYZ::PolyNode* node) {
			if (flag == 2)
			{
				ClipperLibXYZ::ReversePath(node->Contour);
			}		
			for (ClipperLibXYZ::IntPoint& point : node->Contour)
				point.Z = (int)(z);
		};

		fmesh::loopPolyTree(func1, &treeNew);
		adjustPolyTreeZ(treeNew);
	}

	void GeneratorImpl::_buildTop(ClipperLibXYZ::PolyTree& treeTop, double& hTop, double offset)
	{
		hTop = m_adParam.total_height;
		double thickness = m_adParam.extend_width / 2.0f;

		if (m_adParam.top_type == ADTopType::adtt_close
			|| m_adParam.top_type == ADTopType::adtt_round)
			hTop -= m_adParam.bottom_layers >0?m_adParam.bottom_extend_width * m_adParam.bottom_layers : m_adParam.top_extend_width;
		else if (m_adParam.top_type == ADTopType::adtt_step)
			hTop -= (m_adParam.top_height + m_adParam.top_extend_width);

		offsetAndExtendpolyType(m_poly, offset, thickness, hTop, treeTop,m_adParam.bluntSharpCorners);

		if (m_adParam.top_type == ADTopType::adtt_none)
			_fillPolyTree(&treeTop);
		else if (m_adParam.top_type == ADTopType::adtt_close)
		{
			ClipperLibXYZ::PolyTree closeTop;
			double hCloseTop = m_adParam.total_height;
			offsetAndExtendpolyType(m_poly, offset, thickness, hCloseTop, closeTop,m_adParam.bluntSharpCorners);
			_fillPolyTreeDepth14(&closeTop);
			_fillPolyTreeDepth23(&treeTop,true);
			_buildFromSamePolyTree(&treeTop, &closeTop, 3);
		}
		else if (m_adParam.top_type == ADTopType::adtt_step)
		{
			float offsetH = m_adParam.extend_width;
			if (m_adParam.extend_width >= 1)
			{
				offsetH = 0.5;
			}
			std::vector<ClipperLibXYZ::PolyTree> botomSteppolys(3);
			offsetAndExtendpolyType(m_poly, offset, thickness, m_adParam.total_height - m_adParam.top_height, treeTop, m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, m_adParam.total_height - m_adParam.top_height + offsetH, botomSteppolys.at(0), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, m_adParam.total_height - m_adParam.top_height + offsetH, botomSteppolys.at(1), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, m_adParam.total_height, botomSteppolys.at(2), m_adParam.bluntSharpCorners);
			offsetExteriorInner(botomSteppolys.at(0), m_adParam.top_extend_width, m_adParam.total_height - m_adParam.top_height + offsetH);

			double delta = 1.0;
			_buildFromSamePolyTree(&botomSteppolys.at(1), &botomSteppolys.at(2));
			_buildFromDiffPolyTree_diffSafty(&treeTop, &botomSteppolys.at(1), delta, 3);//outer
			_buildFromDiffPolyTree_Inner(&treeTop, &botomSteppolys.at(0), delta, 2, true);//inner
			_buildFromDiffPolyTree_xor(&botomSteppolys.at(0), &botomSteppolys.at(1), delta, 2);
			_fillPolyTree(&botomSteppolys.at(2));
		}
		else if (m_adParam.top_type == ADTopType::adtt_round)
		{
			//

		}

		if (m_topTree)
		{
			if (m_exportParam.top_offset != 0)
			{
				offsetAndExtendpolyType(m_poly, offset + m_exportParam.top_offset, thickness, hTop, *m_topTree,m_adParam.bluntSharpCorners);
			}
		}
	}

	void GeneratorImpl::_buildBottom(ClipperLibXYZ::PolyTree& treeBottom, double& hBottom, double offset)
	{
		hBottom = m_adParam.bottom_offset;
		double thickness = m_adParam.extend_width / 2.0f;

		if (m_adParam.bottom_type == ADBottomType::adbt_close
			|| m_adParam.bottom_type == ADBottomType::adbt_extend_inner
			|| m_adParam.bottom_type == ADBottomType::adbt_extend_outter)
			hBottom += m_adParam.bottom_layers>0? m_adParam.bottom_extend_width* m_adParam.bottom_layers: m_adParam.bottom_extend_width;
		else if (m_adParam.bottom_type == ADBottomType::adbt_step)
			hBottom += (m_adParam.bottom_height + m_adParam.bottom_extend_width);

		offsetAndExtendpolyType(m_poly, offset, thickness, hBottom, treeBottom,m_adParam.bluntSharpCorners);

		if (m_adParam.bottom_type == ADBottomType::adbt_none)
			_fillPolyTree(&treeBottom, true);
		else if (m_adParam.bottom_type == ADBottomType::adbt_close)
		{
			ClipperLibXYZ::PolyTree closeBottom;
			double hCloseBottom = m_adParam.bottom_offset;
			offsetAndExtendpolyType(m_poly, offset, thickness, hCloseBottom, closeBottom,m_adParam.bluntSharpCorners);

			_fillPolyTreeDepth14(&closeBottom, true);
			_fillPolyTreeDepth23(&treeBottom);
			_buildFromSamePolyTree(&closeBottom, &treeBottom, 3);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
			std::vector<ClipperLibXYZ::PolyTree> botomSteppolys(3);
			double h = (m_adParam.bottom_height - m_adParam.extend_width) > 0 ? (m_adParam.bottom_height - m_adParam.extend_width) : 0;
			offsetAndExtendpolyType(m_poly, offset, thickness, 0, botomSteppolys.at(0),m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, h, botomSteppolys.at(1), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, m_adParam.bottom_height, botomSteppolys.at(2), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, m_adParam.bottom_height, treeBottom, m_adParam.bluntSharpCorners);
			offsetExteriorInner(botomSteppolys.at(2), m_adParam.bottom_extend_width, m_adParam.bottom_height);

			double delta = 1.0;
			_buildFromSamePolyTree(&botomSteppolys.at(0), &botomSteppolys.at(1));
			_buildFromDiffPolyTree_diffSafty(&botomSteppolys.at(1), &treeBottom, delta, 3);//outer
			_buildFromDiffPolyTree_Inner(&botomSteppolys.at(1), &botomSteppolys.at(2), delta, 2, true);//inner
			_buildFromDiffPolyTree_xor(&botomSteppolys.at(2), &treeBottom, delta, 2);
			_fillPolyTree(&botomSteppolys.at(0), true);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_extend_inner)
		{
			std::vector<ClipperLibXYZ::PolyTree> polys(2);
			offsetAndExtendpolyType(m_poly, offset, thickness, 0, polys.at(0), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, thickness, polys.at(1), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, thickness, treeBottom, m_adParam.bluntSharpCorners);
			offsetExteriorInner(polys.at(0), m_adParam.bottom_extend_width,0.0);
			offsetExteriorInner(polys.at(1), m_adParam.bottom_extend_width, m_adParam.extend_width);

			_fillPolyTree(&polys.at(0), true);
			ClipperLibXYZ::PolyTree out;
			fmesh::xor2PolyTrees(&polys.at(1), &treeBottom, out);
			_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
			_buildFromDiffPolyTree_firstLayer(&out);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_extend_outter)
		{
			std::vector<ClipperLibXYZ::PolyTree> polys(2);
			offsetAndExtendpolyType(m_poly, offset, thickness, 0, polys.at(0), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, thickness, polys.at(1), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, thickness, treeBottom, m_adParam.bluntSharpCorners);
			offsetExterior(polys.at(0), m_adParam.bottom_extend_width / 2.0f,0.0);
			offsetExterior(polys.at(1), m_adParam.bottom_extend_width / 2.0f, m_adParam.extend_width);

			_fillPolyTree(&polys.at(0), true);
			ClipperLibXYZ::PolyTree out;
			fmesh::xor2PolyTrees(&polys.at(1), &treeBottom, out);
			_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
			_buildFromDiffPolyTree_firstLayer(&out);
		}

		if (m_bottomTree)
		{
			if (m_exportParam.bottom_offset != 0)
			{
				offsetAndExtendPolyTreeNew(m_poly, m_adParam.bottom_offset + offset, thickness, hBottom, *m_bottomTree);
			}
		}
	}

	void GeneratorImpl::_buildTopDiff(ClipperLibXYZ::PolyTree& treeTop, ClipperLibXYZ::PolyTree* top)
	{
		double hTop = m_adParam.total_height;
		double thickness = m_adParam.extend_width / 2.0f;

		if (m_adParam.top_type == ADTopType::adtt_close
			|| m_adParam.top_type == ADTopType::adtt_round)
			hTop -= m_adParam.top_layers > 0 ? m_adParam.top_extend_width * m_adParam.top_layers : m_adParam.top_extend_width;
		else if (m_adParam.top_type == ADTopType::adtt_step)
			hTop -= (m_adParam.top_height + m_adParam.top_extend_width);

		copy2PolyTree(*top, treeTop);
		setPolyTreeZ(treeTop, hTop);
		//offsetAndExtendPolyTree(m_poly, offset, thickness, hTop, treeTop);

		if (m_adParam.top_type == ADTopType::adtt_none)
			_fillPolyTree(&treeTop);
		else if (m_adParam.top_type == ADTopType::adtt_close)
		{
			ClipperLibXYZ::PolyTree closeTop;
			double hCloseTop = m_adParam.total_height;

			copy2PolyTree(*top, closeTop);
			setPolyTreeZ(closeTop, hCloseTop);
			//offsetAndExtendPolyTree(m_poly, offset, thickness, hCloseTop, closeTop);

			_fillPolyTreeDepth14(&closeTop);
			_fillPolyTreeDepth23(&treeTop, true);
			_buildFromSamePolyTree(&treeTop, &closeTop, 3);
		}
		else if (m_adParam.top_type == ADTopType::adtt_step)
		{
			float offsetH = m_adParam.extend_width;
			if (m_adParam.extend_width >= 1)
			{
				offsetH = 0.5;
			}
			std::vector<ClipperLibXYZ::PolyTree> botomSteppolys(3);

			copy2PolyTree(*top, treeTop);
			setPolyTreeZ(treeTop, m_adParam.total_height - m_adParam.top_height);
			copy2PolyTree(*top, botomSteppolys.at(0));
			setPolyTreeZ(botomSteppolys.at(0), m_adParam.total_height - m_adParam.top_height + offsetH);
			copy2PolyTree(*top, botomSteppolys.at(1));
			setPolyTreeZ(botomSteppolys.at(1), m_adParam.total_height - m_adParam.top_height + offsetH);
			copy2PolyTree(*top, botomSteppolys.at(2));
			setPolyTreeZ(botomSteppolys.at(2), m_adParam.total_height);
			//fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.total_height - m_adParam.top_height, treeTop);
			//fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.total_height - m_adParam.top_height + offsetH, botomSteppolys.at(0));
			//fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.total_height - m_adParam.top_height + offsetH, botomSteppolys.at(1));
			//fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.total_height, botomSteppolys.at(2));
			offsetExteriorInner(botomSteppolys.at(0), m_adParam.top_extend_width, m_adParam.total_height - m_adParam.top_height + offsetH);

			double delta = 1.0;
			_buildFromSamePolyTree(&botomSteppolys.at(1), &botomSteppolys.at(2));
			_buildFromDiffPolyTree_diffSafty(&treeTop, &botomSteppolys.at(1), delta, 3);//outer
			_buildFromDiffPolyTree_xor(&treeTop, &botomSteppolys.at(0), delta, 2, true);//inner
			_buildFromDiffPolyTree_xor(&botomSteppolys.at(0), &botomSteppolys.at(1), delta, 2);
			_fillPolyTree(&botomSteppolys.at(2));
		}
		else if (m_adParam.top_type == ADTopType::adtt_round)
		{
			//

		}

		//if (m_topTree)
		//{
		//	if (m_exportParam.top_offset != 0)
		//	{
		//		offsetAndExtendPolyTree(m_poly, offset + m_exportParam.top_offset, thickness, hTop, *m_topTree);
		//	}
		//}
	}
	void GeneratorImpl::_buildBottomDiff(ClipperLibXYZ::PolyTree& treeBottom, ClipperLibXYZ::PolyTree* bottom)
	{
		double hBottom = m_adParam.bottom_offset;
		double thickness = m_adParam.extend_width / 2.0f;

		if (m_adParam.bottom_type == ADBottomType::adbt_close
			|| m_adParam.bottom_type == ADBottomType::adbt_extend_inner
			|| m_adParam.bottom_type == ADBottomType::adbt_extend_outter)
			hBottom += m_adParam.bottom_layers > 0 ? m_adParam.bottom_extend_width * m_adParam.bottom_layers : m_adParam.bottom_extend_width;
		else if (m_adParam.bottom_type == ADBottomType::adbt_step)
			hBottom += (m_adParam.bottom_height + m_adParam.bottom_extend_width);

		copy2PolyTree(*bottom, treeBottom);
		setPolyTreeZ(treeBottom, hBottom);
		//offsetAndExtendPolyTree(m_poly, offset, thickness, hBottom, treeBottom);

		if (m_adParam.bottom_type == ADBottomType::adbt_none)
			_fillPolyTree(&treeBottom, true);
		else if (m_adParam.bottom_type == ADBottomType::adbt_close)
		{
			ClipperLibXYZ::PolyTree closeBottom;
			double hCloseBottom = m_adParam.bottom_offset;

			copy2PolyTree(*bottom, closeBottom);
			setPolyTreeZ(closeBottom, hCloseBottom);
			//offsetAndExtendPolyTree(m_poly, offset, thickness, hCloseBottom, closeBottom);

			_fillPolyTreeDepth14(&closeBottom, true);
			_fillPolyTreeDepth23(&treeBottom);
			_buildFromSamePolyTree(&closeBottom, &treeBottom, 3);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
			std::vector<ClipperLibXYZ::PolyTree> botomSteppolys(3);
			double h = (m_adParam.bottom_height - m_adParam.extend_width) > 0 ? (m_adParam.bottom_height - m_adParam.extend_width) : 0;
			
			copy2PolyTree(*bottom, botomSteppolys.at(0));
			setPolyTreeZ(botomSteppolys.at(0), 0.0);
			copy2PolyTree(*bottom, botomSteppolys.at(1));
			setPolyTreeZ(botomSteppolys.at(1), h);
			copy2PolyTree(*bottom, botomSteppolys.at(2));
			setPolyTreeZ(botomSteppolys.at(2), m_adParam.bottom_height);
			copy2PolyTree(*bottom, treeBottom);
			setPolyTreeZ(treeBottom, m_adParam.bottom_height);
			//fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, 0, botomSteppolys.at(0));
			//fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, h, botomSteppolys.at(1));
			//fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.bottom_height, botomSteppolys.at(2));
			//fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.bottom_height, treeBottom);
			offsetExteriorInner(botomSteppolys.at(2), m_adParam.bottom_extend_width, m_adParam.bottom_height);

			double delta = 1.0;
			_buildFromSamePolyTree(&botomSteppolys.at(0), &botomSteppolys.at(1));
			_buildFromDiffPolyTree_diffSafty(&botomSteppolys.at(1), &treeBottom, delta, 3);//outer
			_buildFromDiffPolyTree_xor(&botomSteppolys.at(1), &botomSteppolys.at(2), delta, 2, true);//inner
			_buildFromDiffPolyTree_xor(&botomSteppolys.at(2), &treeBottom, delta, 2);
			_fillPolyTree(&botomSteppolys.at(0), true);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_extend_inner)
		{
			std::vector<ClipperLibXYZ::PolyTree> polys(2);

			copy2PolyTree(*bottom, polys.at(0));
			setPolyTreeZ(polys.at(0), 0.0);
			copy2PolyTree(*bottom, polys.at(1));
			setPolyTreeZ(polys.at(1), m_adParam.extend_width);
			copy2PolyTree(*bottom, treeBottom);
			setPolyTreeZ(treeBottom, m_adParam.extend_width);
			//offsetAndExtendPolyTree(m_poly, offset, thickness, 0, polys.at(0));
			//offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.extend_width, polys.at(1));
			//offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.extend_width, treeBottom);
			offsetExteriorInner(polys.at(0), m_adParam.bottom_extend_width, 0.0);
			offsetExteriorInner(polys.at(1), m_adParam.bottom_extend_width, m_adParam.extend_width);

			_fillPolyTree(&polys.at(0), true);
			ClipperLibXYZ::PolyTree out;
			fmesh::xor2PolyTrees(&polys.at(1), &treeBottom, out);
			_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
			_buildFromDiffPolyTree_firstLayer(&out);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_extend_outter)
		{
			std::vector<ClipperLibXYZ::PolyTree> polys(2);

			copy2PolyTree(*bottom, polys.at(0));
			setPolyTreeZ(polys.at(0), 0.0);
			copy2PolyTree(*bottom, polys.at(1));
			setPolyTreeZ(polys.at(1), m_adParam.extend_width);
			copy2PolyTree(*bottom, treeBottom);
			setPolyTreeZ(treeBottom, m_adParam.extend_width);
			//offsetAndExtendPolyTree(m_poly, offset, thickness, 0, polys.at(0));
			//offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.extend_width, polys.at(1));
			//offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.extend_width, treeBottom);
			offsetExterior(polys.at(0), m_adParam.bottom_extend_width / 2.0f, 0.0);
			offsetExterior(polys.at(1), m_adParam.bottom_extend_width / 2.0f, m_adParam.extend_width);

			_fillPolyTree(&polys.at(0), true);
			ClipperLibXYZ::PolyTree out;
			fmesh::xor2PolyTrees(&polys.at(1), &treeBottom, out);
			_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
			_buildFromDiffPolyTree_firstLayer(&out);
		}

		//if (m_bottomTree)
		//{
		//	if (m_exportParam.bottom_offset != 0)
		//	{
		//		offsetAndExtendPolyTreeNew(m_poly, m_adParam.bottom_offset + offset, thickness, hBottom, *m_bottomTree);
		//	}
		//}
	}

	void GeneratorImpl::_buildTopBottom(ClipperLibXYZ::PolyTree* treeBottom, ClipperLibXYZ::PolyTree* treeTop, double offsetB, double offsetT)
	{
		ClipperLibXYZ::PolyTree _treeTop, _treeBottom;
		double hTop, hBottom;

		if (treeBottom != nullptr)
		{
			_buildBottom(_treeBottom, hBottom, offsetB);
			_buildFromDiffPolyTree(&_treeBottom, treeBottom);
		}
		if (treeTop != nullptr)
		{
			_buildTop(_treeTop, hTop, offsetT);
			_buildFromDiffPolyTree(treeTop, &_treeTop);
		}
	}

	void GeneratorImpl::_buildTopBottomDiff(ClipperLibXYZ::PolyTree* treeBottom, ClipperLibXYZ::PolyTree* treeTop, double offsetB, double offsetT)
	{
		ClipperLibXYZ::PolyTree _treeTop, _treeBottom;
		double hTop, hBottom;

		if (treeBottom != nullptr)
		{
			_buildBottomDiff(_treeBottom, treeBottom);
			_buildFromDiffPolyTree(&_treeBottom, treeBottom);
		}
		if (treeTop != nullptr)
		{
			_buildTopDiff(_treeTop, treeTop);
			_buildFromDiffPolyTree(treeTop, &_treeTop);
		}
	}

	void GeneratorImpl::_buildTopBottom_onepoly(ClipperLibXYZ::PolyTree* treeBottom, ClipperLibXYZ::PolyTree* treeTop, double offsetB /*= 0*/, double offsetT /*= 0*/)
	{
		ClipperLibXYZ::PolyTree _treeTop, _treeBottom;
		double hTop, hBottom;

		if (treeBottom != nullptr)
		{
			_buildBottom_onepoly(_treeBottom, hBottom, offsetB);
			_buildFromDiffPolyTree(&_treeBottom, treeBottom);
		}
		if (treeTop != nullptr)
		{
			_buildTop_onepoly(_treeTop, hTop, offsetT);
			_buildFromDiffPolyTree(treeTop, &_treeTop);
		}
	}

	void GeneratorImpl::_buildTop_onepoly(ClipperLibXYZ::PolyTree& treeTop, double& hTop, double offset /*= 0*/)
	{
		hTop = m_adParam.total_height;
		double thickness = m_adParam.extend_width / 2.0f;
		thickness = 0.0;
		if (m_adParam.top_type == ADTopType::adtt_close)
			;// hTop -= m_adParam.top_height;
		else if (m_adParam.top_type == ADTopType::adtt_round)
			;// hTop -= m_adParam.top_height;
		else if (m_adParam.top_type == ADTopType::adtt_step)
			;// hTop -= (m_adParam.top_height + m_adParam.extend_width / 4.0 + m_adParam.top_extend_width / 4.0);

		hTop = hTop < 0 ? 0 : hTop;

		//offsetAndExtendPolyTree(m_poly, offset, thickness, hTop, treeTop);
		offsetPolyType(m_poly, offset, treeTop, m_adParam.bluntSharpCorners);

		//copy2PolyTree(m_poly, treeTop);
		setPolyTreeZ(treeTop, hTop);

		if (m_adParam.top_type == ADTopType::adtt_none)
			;//_fillPolyTree(&treeTop);
		else if (m_adParam.top_type == ADTopType::adtt_close)
		{
			//std::vector<ClipperLibXYZ::PolyTree> poly(1);
			//copy2PolyTree(m_poly, poly.at(0));
			//setPolyTreeZ(poly.at(0), m_adParam.total_height);
			//_buildFromSamePolyTree(&treeTop, &poly.at(0));
			//_fillPolyTreeDepthOnePoly(&treeTop, true);
			//_fillPolyTreeDepthOnePoly(&poly.at(0));
			_fillPolyTreeOnePloy(&treeTop);
		}
		else if (m_adParam.top_type == ADTopType::adtt_step)
		{
			//double h = (m_adParam.total_height - m_adParam.top_height) > 0 ? (m_adParam.total_height - m_adParam.top_height) : 0;
			//int count = m_adParam.extend_width > 0 ? (0.5 + 1.0 * m_adParam.top_extend_width / m_adParam.extend_width) : 1;
			//std::vector<ClipperLibXYZ::PolyTree> botomSteppolys(4);
			//offsetPolyTree(m_poly, -count* m_adParam.extend_width, botomSteppolys.at(0));
			//setPolyTreeZ(botomSteppolys.at(0), h);

			//copy2PolyTree(m_poly, botomSteppolys.at(1));
			//setPolyTreeZ(botomSteppolys.at(1), h);

			//copy2PolyTree(m_poly, botomSteppolys.at(2));
			//setPolyTreeZ(botomSteppolys.at(2), m_adParam.total_height);

			//offsetPolyTree(m_poly, -count * m_adParam.extend_width, botomSteppolys.at(3));
			//setPolyTreeZ(botomSteppolys.at(3), h-0.2);

			//_buildFromSamePolyTree(&treeTop, &botomSteppolys.at(1));
			//_buildFromSamePolyTree(&botomSteppolys.at(1), &botomSteppolys.at(2));
			//_buildFromSamePolyTree(&botomSteppolys.at(0), &botomSteppolys.at(3));

			//std::vector<ClipperLibXYZ::PolyTree> offsetPolys(2);
			//offsetPolyTree(m_poly, -count * m_adParam.extend_width - 0.2, offsetPolys.at(0));
			//setPolyTreeZ(offsetPolys.at(0), h);

			//offsetPolyTree(m_poly, -m_adParam.extend_width - 0.2, offsetPolys.at(1));
			//setPolyTreeZ(offsetPolys.at(1), hTop);
			//_buildFromDiffPolyTree_diffSafty(&offsetPolys.at(1), &offsetPolys.at(0));
		}
		else if (m_adParam.top_type == ADTopType::adtt_round)
		{
			//

		}

		if (m_topTree)
		{
			if (m_exportParam.top_offset != 0)
			{
				offsetAndExtendpolyType(m_poly, offset + m_exportParam.top_offset, thickness, hTop, *m_topTree,m_adParam.bluntSharpCorners);
			}
		}
	}

	void GeneratorImpl::_buildBottom_onepoly(ClipperLibXYZ::PolyTree& treeBottom, double& hBottom, double offset /*= 0*/)
	{
		hBottom = m_adParam.bottom_offset;
		double thickness = m_adParam.extend_width / 2.0f;

		if (m_adParam.bottom_type == ADBottomType::adbt_close)
			;
		else if (m_adParam.bottom_type == ADBottomType::adbt_extend_inner
			|| m_adParam.bottom_type == ADBottomType::adbt_extend_outter)
			hBottom = 0;//hBottom += m_adParam.bottom_height;
		else if (m_adParam.bottom_type == ADBottomType::adbt_step)
			;// hBottom = m_adParam.bottom_height;

		//offsetAndExtendPolyTree(m_poly, offset, thickness, hBottom, treeBottom);
		offsetPolyType(m_poly, offset, treeBottom, m_adParam.bluntSharpCorners);

		//copy2PolyTree(m_poly, treeBottom);
		setPolyTreeZ(treeBottom, hBottom);

		if (m_adParam.bottom_type == ADBottomType::adbt_none)
			;//_fillPolyTree(&treeBottom, true);
		else if (m_adParam.bottom_type == ADBottomType::adbt_close)
		{
			//_fillPolyTreeDepthOnePoly(&treeBottom, true);
			//_fillPolyTree(&treeBottom, true);
			_fillPolyTreeOnePloy(&treeBottom,true);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
			//double h = (m_adParam.bottom_height - m_adParam.extend_width / 4.0f - m_adParam.bottom_extend_width / 4.0) ? (m_adParam.bottom_height - m_adParam.extend_width / 4.0 - m_adParam.bottom_extend_width / 4.0) : 0.5;
			//int count = m_adParam.extend_width > 0 ? (0.5 + 1.0 * m_adParam.bottom_extend_width / m_adParam.extend_width) : 1;
			//std::vector<ClipperLibXYZ::PolyTree> botomSteppolys(4);
			//offsetPolyTree(m_poly, -count* m_adParam.extend_width, botomSteppolys.at(0));
			//setPolyTreeZ(botomSteppolys.at(0), hBottom);

			//copy2PolyTree(m_poly, botomSteppolys.at(1));
			//setPolyTreeZ(botomSteppolys.at(1), h);

			//offsetPolyTree(m_poly, -count * m_adParam.extend_width, botomSteppolys.at(2));
			//setPolyTreeZ(botomSteppolys.at(2), hBottom-0.2);

			//copy2PolyTree(m_poly, botomSteppolys.at(3));
			//setPolyTreeZ(botomSteppolys.at(3), 0.0);

			//_buildFromSamePolyTree(&botomSteppolys.at(3), &botomSteppolys.at(1));
			//_buildFromSamePolyTree(&botomSteppolys.at(1), &treeBottom);
			//_buildFromSamePolyTree(&botomSteppolys.at(0), &botomSteppolys.at(2));

			//std::vector<ClipperLibXYZ::PolyTree> offsetPolys(2);
			//offsetPolyTree(m_poly, -count * m_adParam.extend_width - 0.2, offsetPolys.at(0));
			//setPolyTreeZ(offsetPolys.at(0), hBottom);

			//offsetPolyTree(m_poly, -m_adParam.extend_width - 0.2, offsetPolys.at(1));
			//setPolyTreeZ(offsetPolys.at(1), h);
			//_buildFromDiffPolyTree_diffSafty(&offsetPolys.at(1), &offsetPolys.at(0));
			////_buildFromDiffPolyTree_xor(&botomSteppolys.at(0), &treeBottom);

		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_extend_inner)
		{
			int count = m_adParam.extend_width > 0 ? (0.5 + 1.0 * m_adParam.bottom_extend_width / m_adParam.extend_width) : 1;
			std::vector<ClipperLibXYZ::PolyTree> polysTop(count);
			std::vector<ClipperLibXYZ::PolyTree> polysBom(count);
			float offset = 0.0;
			for (size_t i = 0; i < count; i++)
			{		
				if (i % 2 > 0)
				{
					offset += m_adParam.extend_width / 2.0;
				}
				else
					offset += m_adParam.extend_width;

				offsetPolyTree(m_poly, -offset, polysTop.at(i));
				setPolyTreeZ(polysTop.at(i), 0.2);

				offsetPolyTree(m_poly, -offset, polysBom.at(i));
				setPolyTreeZ(polysBom.at(i), 0.0);

				if (i % 2 == 0)
					_buildFromSamePolyTree(&polysTop.at(i), &polysBom.at(i));
				else
					_buildFromSamePolyTree(&polysBom.at(i), &polysTop.at(i));
			}

		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_extend_outter)
		{
			int count = m_adParam.extend_width > 0 ? (0.5 + 1.0 * m_adParam.bottom_extend_width / m_adParam.extend_width) : 1;
			std::vector<ClipperLibXYZ::PolyTree> polysTop(count);
			std::vector<ClipperLibXYZ::PolyTree> polysBom(count);
			float offset = 0.0;
			for (size_t i = 0; i < count; i++)
			{		
				if (i % 2)
				{
					offset += m_adParam.extend_width /2.0;
				}
				else
					offset += m_adParam.extend_width;

				offsetPolyTree(m_poly, offset, polysTop.at(i));
				setPolyTreeZ(polysTop.at(i), 0.2);

				offsetPolyTree(m_poly, offset, polysBom.at(i));
				setPolyTreeZ(polysBom.at(i), 0.0);
				if (i%2==0)
				{
					_buildFromSamePolyTree(&polysTop.at(i), &polysBom.at(i));
				} 
				else
				{
					_buildFromSamePolyTree(&polysBom.at(i), &polysTop.at(i));
				}

			}
		}

		if (m_bottomTree)
		{
			if (m_exportParam.bottom_offset != 0)
			{
				offsetAndExtendpolyType(m_poly, m_adParam.bottom_offset + offset, thickness, hBottom, *m_bottomTree,m_adParam.bluntSharpCorners);
			}
		}
	}

	void GeneratorImpl::_simplifyPoly(ClipperLibXYZ::PolyTree* poly, double distance/*=0*/)
	{
		double x = dmax.x - dmin.x;
		double y = dmax.y - dmin.y;
		size_t childcount = poly->ChildCount();
		if (!distance)
			 distance = ((x * y / 10000) / childcount) > 1 ? (x * y / 10000) / childcount : 1.415;

		polyNodeFunc func = [&distance](ClipperLibXYZ::PolyNode* node) {
			ClipperLibXYZ::CleanPolygon(node->Contour, distance);
		};
		fmesh::loopPolyTree(func, poly);
	}

	void GeneratorImpl::_simplifyPolyOneploy(ClipperLibXYZ::PolyTree* poly, bool outer, double distance)
	{
		double x = dmax.x - dmin.x;
		double y = dmax.y - dmin.y;
		size_t childcount = poly->ChildCount();
		if (!distance)
			distance = ((x * y / 10000) / childcount) > 1 ? (x * y / 10000) / childcount : 1.415;
		int flag = 3;
		if (!outer)
		{
			flag = 2;
		}
		polyNodeFunc func = [&distance,&flag](ClipperLibXYZ::PolyNode* node) {
			if (checkFlag(node, flag))
				ClipperLibXYZ::CleanPolygon(node->Contour, distance);
		};
		fmesh::loopPolyTree(func, poly);
	}

	void GeneratorImpl::areaPoly(ClipperLibXYZ::PolyTree& poly, std::vector<double>& area)
	{
		double _area = 0.0;
		polyNodeFunc func1 = [&func1, &_area](ClipperLibXYZ::PolyNode* node) {
			_area += ClipperLibXYZ::Area(node->Contour);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func1(n);
		};

		for (ClipperLibXYZ::PolyNode* n : poly.Childs)
		{
			_area = 0.0;
			func1(n);
			area.push_back(_area / 1000000.f);
		}
	}
}