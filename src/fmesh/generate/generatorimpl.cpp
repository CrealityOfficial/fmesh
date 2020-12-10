#include "generatorimpl.h"
#include "fmesh/contour/polytree.h"
#include "fmesh/generate/trimeshbuilder.h"
#include "fmesh/roof/roof.h"

namespace fmesh
{
	GeneratorImpl::GeneratorImpl()
		:m_paths(nullptr)
	{

	}

	GeneratorImpl::~GeneratorImpl()
	{
	}

	trimesh::TriMesh* GeneratorImpl::build(ClipperLib::Paths* paths, const GenParam& param, const F2MParam& modelparam, const Args& args)
	{
		m_paths = paths;
		m_param = param;
		m_modelparam = modelparam;
		m_args = args;

		fmesh::convertPaths2PolyTree(m_paths, m_poly);
		if (!paths || paths->size() == 0)
			return nullptr;

		build();

		trimesh::TriMesh* mesh = generateFromPatches();
		releaseResources();
		return mesh;
	}

	trimesh::TriMesh* GeneratorImpl::generate(ClipperLib::Paths* paths, const ADParam& param)
	{
		m_paths = paths;
		m_adParam = param;

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

	void GeneratorImpl::_fillPolyTreeReverseInner(ClipperLib::PolyTree* tree, bool invert /*= false*/)
	{
		std::vector<Patch*> patches;
		fillComplexPolyTreeReverseInner(tree, patches);

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
		buildRoofs(polyTree, patches, roofHeight, thickness);
		addPatches(patches);
	}
}