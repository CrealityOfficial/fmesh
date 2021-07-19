#include "generatorimpl.h"
#include "fmesh/contour/polytree.h"
#include "fmesh/generate/trimeshbuilder.h"
#include "fmesh/contour/path.h"

#include "mmesh/cgal/roof.h"
#include "mmesh/clipper/circurlar.h"

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

	trimesh::TriMesh* GeneratorImpl::generate(ClipperLib::Paths* paths, const ADParam& param)
	{
		m_paths = paths;
		m_adParam = param;

		ClipperLib::IntPoint bmin;
		ClipperLib::IntPoint bmax;
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

	trimesh::TriMesh* GeneratorImpl::generateShell(ClipperLib::Paths* paths, const ADParam& param,
		ExportParam* exportParam, ClipperLib::PolyTree* topTree, ClipperLib::PolyTree* bottomTree)
	{
		m_paths = paths;
		m_adParam = param;

		if (exportParam)
			m_exportParam = *exportParam;
		m_topTree = topTree;
		m_bottomTree = bottomTree;

		ClipperLib::IntPoint bmin;
		ClipperLib::IntPoint bmax;
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

	void GeneratorImpl::setup(const ADParam& param, ClipperLib::Paths* paths, mmesh::StatusTracer* tracer)
	{
		m_tracer = tracer;

		m_paths = paths;
		m_adParam = param;

		ClipperLib::IntPoint bmin;
		ClipperLib::IntPoint bmax;
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

	void GeneratorImpl::generateBoard(const ExportParam& param, ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		m_exportParam = param;
		topTree.Clear();
		bottomTree.Clear();
		buildBoard(topTree, bottomTree);
	}

	trimesh::TriMesh* GeneratorImpl::generate(ClipperLib::PolyTree* tree, const ADParam& param,
		ExportParam* exportParam, ClipperLib::PolyTree* topTree, ClipperLib::PolyTree* bottomTree)
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
		//ClipperLib::IntPoint bmin;
		//ClipperLib::IntPoint bmax;
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

	void GeneratorImpl::_fillPolyTree(ClipperLib::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;
		fillComplexPolyTree(tree, patches);

		addPatches(patches, invert);
	}

	void GeneratorImpl::_fillPolyTreeDepth14(ClipperLib::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;

		fillPolyTreeDepth14(tree, patches);
		addPatches(patches, invert);
	}

	void GeneratorImpl::_fillPolyTreeDepth23(ClipperLib::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;

		fillPolyTreeDepth23(tree, patches);
		addPatches(patches, invert);
	}

	void GeneratorImpl::_fillPolyTreeInner(ClipperLib::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;
		fillPolyNodeInner(tree, patches);

		addPatches(patches, invert);
	}

	void GeneratorImpl::_buildFromSamePolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag)
	{
		addPatch(buildFromSamePolyTree(treeLower, treeUp, flag));
	}

	void GeneratorImpl::_buildFromDiffPolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag)
	{
		std::vector<Patch*> patches;
		buildFromDiffPolyTreeSafty(treeLower, treeUp, patches, 1.0, flag);
		addPatches(patches);
	}

	void GeneratorImpl::_buildFromDiffPolyTree_diffSafty(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta, int flag, bool invert, bool invertXor)
	{
		ClipperLib::PolyTree out;

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

	void GeneratorImpl::_buildFromDiffPolyTree_diff(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta, int flag, bool invert)
	{
		ClipperLib::PolyTree out;
		std::vector<Patch*> patches;

		xor2PolyTrees(treeLower, treeUp, out);
		if (out.ChildCount() > 0)
		{
			fillComplexPolyTreeReverseInner(&out, patches);
			//_fillPolyTreeReverse(&out, true);
			addPatches(patches);
		}
	}

	void GeneratorImpl::_buildFromDiffPolyTree_all(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta, int flag,bool invertInner, bool invertOuter)
	{
		ClipperLib::PolyTree treeLowerInner;
		ClipperLib::PolyTree treeUpInner;
		;//2: Inner
		if (GetPolyCount(treeLower, 2) == GetPolyCount(treeUp, 2))
		{
			_buileDetachPloyTree(treeLower, treeLowerInner, delta,2);
			_buileDetachPloyTree(treeUp, treeUpInner, delta,2);
			_buildFromDiffPolyTree_all_same(&treeLowerInner, &treeUpInner, delta, flag, false);
		}
		else
		{
			ClipperLib::PolyTree outInner;
			fmesh::xor2PolyTrees(treeLower, treeUp, outInner, 2);//inner
			_fillPolyTreeReverseInnerNew(&outInner, invertInner);
		}

		//3:outer
		if (GetPolyCount(treeLower, 3) == GetPolyCount(treeUp, 3))
		{
			ClipperLib::PolyTree treeLowerOuter;
			ClipperLib::PolyTree treeUpOuter;

			_buileDetachPloyTree(treeLower, treeLowerOuter, delta,3);
			_buileDetachPloyTree(treeUp, treeUpOuter, delta,3);
			_buildFromDiffPolyTree_all_same(&treeLowerOuter, &treeUpOuter, delta, flag, false);
		}
		else
		{
			ClipperLib::PolyTree outOuter;
			fmesh::xor2PolyTrees(treeLower, treeUp, outOuter, 3);
			_fillPolyTreeReverseInnerNew(&outOuter, invertOuter);
		}

		//if (GetPolyCount(treeLower) == GetPolyCount(treeUp))
		//	_buildFromDiffPolyTree_all_same(treeLower, treeUp, delta, flag, false);
		//else
		//	_buildFromDiffPolyTree_all_diff(treeUp, treeLower, flag);
	}

	void GeneratorImpl::_buildFromDiffPolyTree_all_same(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta, int flag, bool invert)
	{
		std::vector<Patch*> patches;
		buildFromDiffPolyTreeSafty(treeLower, treeUp, patches, delta, flag);
		addPatches(patches, invert);
	}

	void GeneratorImpl::_buildFromDiffPolyTree_xor(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta, int flag, bool invert)
	{
		ClipperLib::PolyTree out;
		//fmesh::xor2PolyTrees(treeUp, treeLower, out, flag);
		std::vector<Patch*> patches;
		xor2PolyTrees(treeUp, treeLower, out, flag);
		if (out.ChildCount() > 0)
		{
			//savePolyTree(&out, "buildFromXOR");
			_fillPolyTreeSameZ(&out, invert);
		}
	}

	void GeneratorImpl::_buildFromDiffPolyTree_Inner(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta /*= 1.0*/, int flag /*= 0*/, bool invert /*= false*/)
	{
		ClipperLib::PolyTree out;
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

	void GeneratorImpl::_buildFromDiffPolyTree_onePoly(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta /*= 1.0*/, int flag /*= 0*/, bool invert /*= false*/)
	{
		ClipperLib::PolyTree out;
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

	void GeneratorImpl::_fillPolyTreeReverseInner(ClipperLib::PolyTree* tree, bool invert /*= false*/)
	{
		std::vector<Patch*> patches;

		//test
		//fillComplexPolyTreeReverseInnerNew(tree, patches, invert);

		fillComplexPolyTreeReverseInner(tree, patches, invert);
		//fillComplexPolyTree(tree, patches);

		addPatches(patches);
	}

	void GeneratorImpl::_fillPolyTreeReverse(ClipperLib::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;
		fillComplexPolyTreeReverse(tree, patches, invert);
		addPatches(patches);
	}

	void GeneratorImpl::_fillPolyTreeReverseInnerNew(ClipperLib::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;

		//test
		fillComplexPolyTreeReverseInnerNew(tree, patches, invert);
		//fillComplexPolyTree(tree, patches);

		addPatches(patches);
	}

	void GeneratorImpl::_fillPolyTreeSameZ(ClipperLib::PolyTree* tree, bool invert /*= false*/)
	{
		std::vector<Patch*> patches;
		//fillComplexPolyTreeReverseInner(tree, patches);
		fillComplexPolyTree(tree, patches);

		addPatches(patches, invert);
	}

	void GeneratorImpl::_dealPolyTreeAxisZ(ClipperLib::PolyTree* tree, double slope, double min, double height)
	{
		dealPolyTreeAxisZ(tree, slope, min, height);
	}

	void GeneratorImpl::_buildFromDiffPolyTree_firstLayer(ClipperLib::PolyTree* treeLower, int flag /*= 0*/)
	{
		std::vector<Patch*> patches;
		fillComplexPolyTree(treeLower, patches);
		addPatches(patches, flag);
	}

	void GeneratorImpl::_buileMergePloyTree(ClipperLib::PolyTree* treeOuter, ClipperLib::PolyTree* treeInner, ClipperLib::PolyTree& treeNew)
	{
		ClipperLib::ClipperOffset offset;
		bool bReverse = false;
		polyNodeFunc func = [&func, &offset, &bReverse](ClipperLib::PolyNode* node) {
			if (bReverse)
				;// ClipperLib::ReversePath(node->Contour);
			offset.AddPath(node->Contour, ClipperLib::jtRound, ClipperLib::EndType::etClosedPolygon);

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};
		func(treeOuter);
		bReverse = true;
		func(treeInner);
		offset.Execute(treeNew, 0);
	}

	void GeneratorImpl::_buileDetachPloyTree(ClipperLib::PolyTree* tree, ClipperLib::PolyTree& treeNew,double delta,int flag)//0 all 2 inner 3 outer
	{
		ClipperLib::ClipperOffset offset;
		double z=0;
		int _flag = flag;
		polyNodeFunc func = [&func, &offset, &_flag, &z](ClipperLib::PolyNode* node) {
			int depth = testPolyNodeDepth(node);
			if ((_flag == 2 && (depth == 2 || depth == 3 || depth == 6 || depth == 7))
				|| (_flag == 3 && (depth == 1 || depth == 4 || depth == 5 || depth == 8))
				|| _flag == 0)
			{
				offset.AddPath(node->Contour, ClipperLib::jtRound, ClipperLib::EndType::etClosedPolygon);
				if (!z && node->Contour.size())
				{
					z = node->Contour.at(0).Z;
				}
			}
			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};
		func(tree);
		offset.Execute(treeNew, 0);

		polyNodeFunc func1 = [&func1, &z,&flag](ClipperLib::PolyNode* node) {
			if (flag == 2)
			{
				ClipperLib::ReversePath(node->Contour);
			}		
			for (ClipperLib::IntPoint& point : node->Contour)
				point.Z = (int)(z);
		};

		mmesh::loopPolyTree(func1, &treeNew);
		adjustPolyTreeZ(treeNew);
	}

	void GeneratorImpl::_buildTop(ClipperLib::PolyTree& treeTop, double& hTop, double offset)
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
			ClipperLib::PolyTree closeTop;
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
			std::vector<ClipperLib::PolyTree> botomSteppolys(3);
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

	void GeneratorImpl::_buildBottom(ClipperLib::PolyTree& treeBottom, double& hBottom, double offset)
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
			ClipperLib::PolyTree closeBottom;
			double hCloseBottom = m_adParam.bottom_offset;
			offsetAndExtendpolyType(m_poly, offset, thickness, hCloseBottom, closeBottom,m_adParam.bluntSharpCorners);

			_fillPolyTreeDepth14(&closeBottom, true);
			_fillPolyTreeDepth23(&treeBottom);
			_buildFromSamePolyTree(&closeBottom, &treeBottom, 3);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
			std::vector<ClipperLib::PolyTree> botomSteppolys(3);
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
			std::vector<ClipperLib::PolyTree> polys(2);
			offsetAndExtendpolyType(m_poly, offset, thickness, 0, polys.at(0), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, m_adParam.extend_width, polys.at(1), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, m_adParam.extend_width, treeBottom, m_adParam.bluntSharpCorners);
			offsetExteriorInner(polys.at(0), m_adParam.bottom_extend_width,0.0);
			offsetExteriorInner(polys.at(1), m_adParam.bottom_extend_width, m_adParam.extend_width);

			_fillPolyTree(&polys.at(0), true);
			ClipperLib::PolyTree out;
			fmesh::xor2PolyTrees(&polys.at(1), &treeBottom, out);
			_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
			_buildFromDiffPolyTree_firstLayer(&out);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_extend_outter)
		{
			std::vector<ClipperLib::PolyTree> polys(2);
			offsetAndExtendpolyType(m_poly, offset, thickness, 0, polys.at(0), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, m_adParam.extend_width, polys.at(1), m_adParam.bluntSharpCorners);
			offsetAndExtendpolyType(m_poly, offset, thickness, m_adParam.extend_width, treeBottom, m_adParam.bluntSharpCorners);
			offsetExterior(polys.at(0), m_adParam.bottom_extend_width / 2.0f,0.0);
			offsetExterior(polys.at(1), m_adParam.bottom_extend_width / 2.0f, m_adParam.extend_width);

			_fillPolyTree(&polys.at(0), true);
			ClipperLib::PolyTree out;
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

	void GeneratorImpl::_buildTopDiff(ClipperLib::PolyTree& treeTop, ClipperLib::PolyTree* top)
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
			ClipperLib::PolyTree closeTop;
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
			std::vector<ClipperLib::PolyTree> botomSteppolys(3);

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
	void GeneratorImpl::_buildBottomDiff(ClipperLib::PolyTree& treeBottom, ClipperLib::PolyTree* bottom)
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
			ClipperLib::PolyTree closeBottom;
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
			std::vector<ClipperLib::PolyTree> botomSteppolys(3);
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
			std::vector<ClipperLib::PolyTree> polys(2);

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
			ClipperLib::PolyTree out;
			fmesh::xor2PolyTrees(&polys.at(1), &treeBottom, out);
			_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
			_buildFromDiffPolyTree_firstLayer(&out);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_extend_outter)
		{
			std::vector<ClipperLib::PolyTree> polys(2);

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
			ClipperLib::PolyTree out;
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

	void GeneratorImpl::_buildTopBottom(ClipperLib::PolyTree* treeBottom, ClipperLib::PolyTree* treeTop, double offsetB, double offsetT)
	{
		ClipperLib::PolyTree _treeTop, _treeBottom;
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

	void GeneratorImpl::_buildTopBottomDiff(ClipperLib::PolyTree* treeBottom, ClipperLib::PolyTree* treeTop, double offsetB, double offsetT)
	{
		ClipperLib::PolyTree _treeTop, _treeBottom;
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

	void GeneratorImpl::_buildTopBottom_onepoly(ClipperLib::PolyTree* treeBottom, ClipperLib::PolyTree* treeTop, double offsetB /*= 0*/, double offsetT /*= 0*/)
	{
		ClipperLib::PolyTree _treeTop, _treeBottom;
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

	void GeneratorImpl::_buildTop_onepoly(ClipperLib::PolyTree& treeTop, double& hTop, double offset /*= 0*/)
	{
		hTop = m_adParam.total_height;
		double thickness = m_adParam.extend_width / 2.0f;
		thickness = 0.0;
		if (m_adParam.top_type == ADTopType::adtt_close)
			hTop -= m_adParam.top_height;
		else if (m_adParam.top_type == ADTopType::adtt_round)
			hTop -= m_adParam.top_height;
		else if (m_adParam.top_type == ADTopType::adtt_step)
			;// hTop -= (m_adParam.top_height + m_adParam.extend_width / 4.0 + m_adParam.top_extend_width / 4.0);
		if (m_adParam.top_type == ADTopType::adtt_close)
			hTop -= m_adParam.top_height;

		hTop = hTop < 0 ? 0 : hTop;

		//offsetAndExtendPolyTree(m_poly, offset, thickness, hTop, treeTop);
		offsetPolyType(m_poly, offset, treeTop, m_adParam.bluntSharpCorners);

		//copy2PolyTree(m_poly, treeTop);
		setPolyTreeZ(treeTop, hTop);

		if (m_adParam.top_type == ADTopType::adtt_none)
			;//_fillPolyTree(&treeTop);
		else if (m_adParam.top_type == ADTopType::adtt_close)
		{
			//std::vector<ClipperLib::PolyTree> poly(1);
			//copy2PolyTree(m_poly, poly.at(0));
			//setPolyTreeZ(poly.at(0), m_adParam.total_height);
			//_buildFromSamePolyTree(&treeTop, &poly.at(0));
			//_fillPolyTreeDepthOnePoly(&treeTop, true);
			//_fillPolyTreeDepthOnePoly(&poly.at(0));
		}
		else if (m_adParam.top_type == ADTopType::adtt_step)
		{
			//double h = (m_adParam.total_height - m_adParam.top_height) > 0 ? (m_adParam.total_height - m_adParam.top_height) : 0;
			//int count = m_adParam.extend_width > 0 ? (0.5 + 1.0 * m_adParam.top_extend_width / m_adParam.extend_width) : 1;
			//std::vector<ClipperLib::PolyTree> botomSteppolys(4);
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

			//std::vector<ClipperLib::PolyTree> offsetPolys(2);
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

	void GeneratorImpl::_buildBottom_onepoly(ClipperLib::PolyTree& treeBottom, double& hBottom, double offset /*= 0*/)
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
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
			//double h = (m_adParam.bottom_height - m_adParam.extend_width / 4.0f - m_adParam.bottom_extend_width / 4.0) ? (m_adParam.bottom_height - m_adParam.extend_width / 4.0 - m_adParam.bottom_extend_width / 4.0) : 0.5;
			//int count = m_adParam.extend_width > 0 ? (0.5 + 1.0 * m_adParam.bottom_extend_width / m_adParam.extend_width) : 1;
			//std::vector<ClipperLib::PolyTree> botomSteppolys(4);
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

			//std::vector<ClipperLib::PolyTree> offsetPolys(2);
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
			std::vector<ClipperLib::PolyTree> polysTop(count);
			std::vector<ClipperLib::PolyTree> polysBom(count);
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
			std::vector<ClipperLib::PolyTree> polysTop(count);
			std::vector<ClipperLib::PolyTree> polysBom(count);
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

	void GeneratorImpl::_simplifyPoly(ClipperLib::PolyTree* poly, double distance/*=0*/)
	{
		double x = dmax.x - dmin.x;
		double y = dmax.y - dmin.y;
		size_t childcount = poly->ChildCount();
		if (!distance)
			 distance = ((x * y / 10000) / childcount) > 1 ? (x * y / 10000) / childcount : 1.415;

		polyNodeFunc func = [&distance](ClipperLib::PolyNode* node) {
			ClipperLib::CleanPolygon(node->Contour, distance);
		};
		mmesh::loopPolyTree(func, poly);
	}

	void GeneratorImpl::_simplifyPolyOneploy(ClipperLib::PolyTree* poly, bool outer, double distance)
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
		polyNodeFunc func = [&distance,&flag](ClipperLib::PolyNode* node) {
			if (checkFlag(node, flag))
				ClipperLib::CleanPolygon(node->Contour, distance);
		};
		mmesh::loopPolyTree(func, poly);
	}

	void GeneratorImpl::areaPoly(ClipperLib::PolyTree& poly, std::vector<double>& area)
	{
		double _area = 0.0;
		polyNodeFunc func1 = [&func1, &_area](ClipperLib::PolyNode* node) {
			_area += ClipperLib::Area(node->Contour);

			for (ClipperLib::PolyNode* n : node->Childs)
				func1(n);
		};

		for (ClipperLib::PolyNode* n : poly.Childs)
		{
			_area = 0.0;
			func1(n);
			area.push_back(_area / 1000000.f);
		}
	}
}