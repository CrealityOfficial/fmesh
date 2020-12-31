#include "generatorimpl.h"
#include "fmesh/contour/polytree.h"
#include "fmesh/generate/trimeshbuilder.h"
#include "fmesh/contour/path.h"

#include "mmesh/cgal/roof.h"
#include "fmesh/dxf/writedxf.h"

namespace fmesh
{
	GeneratorImpl::GeneratorImpl()
		:m_paths(nullptr)
		, m_topTree(nullptr)
		, m_bottomTree(nullptr)
	{

	}

	GeneratorImpl::~GeneratorImpl()
	{
	}

	trimesh::TriMesh* GeneratorImpl::generate(ClipperLib::Paths* paths, const ADParam& param,
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

		build();

		trimesh::TriMesh* mesh = generateFromPatches();
		releaseResources();
		return mesh;
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

	void GeneratorImpl::_fillPolyTreeOutline(ClipperLib::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;

		fillFirstLevelPolyNode(tree, patches);
		addPatches(patches, invert);
	}

	void GeneratorImpl::_fillPolyTreeDepth14(ClipperLib::PolyTree* tree, bool invert)
	{
		std::vector<Patch*> patches;

		fillPolyTreeDepth14(tree, patches);
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
		buildFromDiffPolyTree(treeLower, treeUp, patches, flag);
		addPatches(patches);
	}
	
	void GeneratorImpl::_buildFromDiffPolyTree_drum(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag, ClipperLib::PolyTree& out)
	{
		std::vector<Patch*> patches;
		buildFromDiffPolyTree_drum(treeLower, treeUp, patches, flag,out);
		if (patches.size())
			addPatches(patches);
	}

	void GeneratorImpl::_buildFromDiffPolyTree_diff(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag)
	{
		ClipperLib::PolyTree out;
		//fmesh::xor2PolyTrees(treeUp, treeLower, out, flag);
		std::vector<Patch*> patches;
		buildFromDiffPolyTree_SameAndDiff(treeLower, treeUp, patches, flag, out);
		if (patches.size())
			addPatches(patches);
		if (out.ChildCount() > 0)
		{
			_fillPolyTreeReverseInner(&out, flag);
		}
	}

	void GeneratorImpl::_fillPolyTreeReverseInner(ClipperLib::PolyTree* tree, bool invert /*= false*/)
	{
		std::vector<Patch*> patches;
		fillComplexPolyTreeReverseInner(tree, patches);
		//fillComplexPolyTree(tree, patches);

		addPatches(patches, invert);
	}

	void GeneratorImpl::_dealPolyTreeAxisZ(ClipperLib::PolyTree* tree, double slope, double height)
	{	
		dealPolyTreeAxisZ(tree, slope, height);
	}

	void GeneratorImpl::_buildFromDiffPolyTree_firstLayer(ClipperLib::PolyTree* treeLower, int flag /*= 0*/)
	{
		std::vector<Patch*> patches;
		fillComplexPolyTree(treeLower,patches);
		addPatches(patches, flag);
	}

	void GeneratorImpl::_buildRoof(ClipperLib::PolyTree* polyTree, double roofHeight, double thickness)
	{
		std::vector<Patch*> patches;
		mmesh::buildRoofs(polyTree, patches, roofHeight, thickness);
		addPatches(patches);
	}

	void GeneratorImpl::_buildTop(ClipperLib::PolyTree& treeTop, double& hTop,double offset)
	{
		hTop = m_adParam.total_height;
		double thickness = m_adParam.extend_width / 2.0f;
		
		if (m_adParam.top_type == ADTopType::adtt_close
			|| m_adParam.top_type == ADTopType::adtt_round)
			hTop -= m_adParam.top_height;
		else if (m_adParam.top_type == ADTopType::adtt_step)
			hTop -= (m_adParam.top_height + m_adParam.top_extend_width);
		
		offsetAndExtendPolyTree(m_poly, offset, thickness, hTop, treeTop);

		if (m_adParam.top_type == ADTopType::adtt_none)
			_fillPolyTree(&treeTop);
		else if (m_adParam.top_type == ADTopType::adtt_close)
		{
			ClipperLib::PolyTree closeTop;
			double hCloseTop = m_adParam.total_height;
			offsetAndExtendPolyTree(m_poly, offset, thickness, hCloseTop, closeTop);

			_fillPolyTreeDepth14(&closeTop);
			_fillPolyTreeInner(&treeTop, true);
			_buildFromSamePolyTree(&treeTop, &closeTop, 3);
		}
		else if(m_adParam.top_type == ADTopType::adtt_step)
		{
			float offsetH = m_adParam.extend_width;
			if (m_adParam.extend_width>=1)
			{
				offsetH = 0.5;
			}
			std::vector<ClipperLib::PolyTree> botomSteppolys(3);
			fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.total_height - m_adParam.top_height, treeTop);
			fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.total_height- m_adParam.top_height+ offsetH, botomSteppolys.at(0));
			fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.total_height - m_adParam.top_height+ offsetH, botomSteppolys.at(1));
			fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.total_height, botomSteppolys.at(2));
			offsetExteriorInner(botomSteppolys.at(0), m_adParam.top_extend_width);

			_buildFromSamePolyTree(&botomSteppolys.at(1), &botomSteppolys.at(2));
			_buildFromDiffPolyTree(&treeTop, &botomSteppolys.at(0));
			_buildFromDiffPolyTree(&botomSteppolys.at(0), &botomSteppolys.at(1));
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
				offsetExteriorInner(*m_bottomTree, m_exportParam.top_offset + offset);
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
			hBottom += m_adParam.bottom_height;
		else if (m_adParam.bottom_type == ADBottomType::adbt_step)
			hBottom += (m_adParam.bottom_height + m_adParam.bottom_extend_width);
				
		offsetAndExtendPolyTree(m_poly, offset, thickness, hBottom, treeBottom);

		if (m_adParam.bottom_type == ADBottomType::adbt_none)
			_fillPolyTree(&treeBottom, true);
		else if (m_adParam.bottom_type == ADBottomType::adbt_close)
		{
			ClipperLib::PolyTree closeBottom;
			double hCloseBottom = m_adParam.bottom_offset;
			offsetAndExtendPolyTree(m_poly, offset, thickness, hCloseBottom, closeBottom);

			_fillPolyTreeDepth14(&closeBottom, true);
			_fillPolyTreeInner(&treeBottom);
			_buildFromSamePolyTree(&closeBottom, &treeBottom, 3);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
			std::vector<ClipperLib::PolyTree> botomSteppolys(3);
			fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, 0, botomSteppolys.at(0));
			fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.bottom_height, botomSteppolys.at(1));
			fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.bottom_height+ m_adParam.extend_width, botomSteppolys.at(2));
			fmesh::offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.bottom_height+m_adParam.extend_width, treeBottom);
			offsetExteriorInner(botomSteppolys.at(2), m_adParam.bottom_extend_width);

			_buildFromSamePolyTree(&botomSteppolys.at(0), &botomSteppolys.at(1));
			_buildFromDiffPolyTree(&botomSteppolys.at(1), &botomSteppolys.at(2));
			_buildFromDiffPolyTree(&botomSteppolys.at(2), &treeBottom);
			_fillPolyTree(&botomSteppolys.at(0), true);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_extend_inner)
		{
			std::vector<ClipperLib::PolyTree> polys(2);
			offsetAndExtendPolyTree(m_poly, offset, thickness, 0, polys.at(0));
			offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.extend_width, polys.at(1));
			offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.extend_width, treeBottom);
			offsetExteriorInner(polys.at(0), m_adParam.bottom_extend_width);
			offsetExteriorInner(polys.at(1), m_adParam.bottom_extend_width);

			_fillPolyTree(&polys.at(0), true);
			ClipperLib::PolyTree out;
			fmesh::xor2PolyTrees(&polys.at(1), &treeBottom, out);
			_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
			_buildFromDiffPolyTree_firstLayer(&out);
		}
		else if (m_adParam.bottom_type == ADBottomType::adbt_extend_outter)
		{
			std::vector<ClipperLib::PolyTree> polys(2);
			offsetAndExtendPolyTree(m_poly, offset, thickness, 0, polys.at(0));
			offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.extend_width, polys.at(1));
			offsetAndExtendPolyTree(m_poly, offset, thickness, m_adParam.extend_width, treeBottom);
			offsetExterior(polys.at(0), m_adParam.bottom_extend_width);
			offsetExterior(polys.at(1), m_adParam.bottom_extend_width);

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
				offsetExteriorInner(*m_bottomTree, m_adParam.bottom_offset + offset);
			}
		}
	}

	void GeneratorImpl::_buildTopBottom(ClipperLib::PolyTree* treeBottom, ClipperLib::PolyTree* treeTop,double offsetB,double offsetT)
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
}