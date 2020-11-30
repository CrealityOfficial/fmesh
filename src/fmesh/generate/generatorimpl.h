#ifndef FMESH_GENERATORIMPL_1604466979729_H
#define FMESH_GENERATORIMPL_1604466979729_H
#include <clipper/clipper.hpp>
#include "trimesh2/TriMesh.h"
#include <map>

#include "fmesh/common/param.h"
#include "fmesh/generate/patch.h"
#include "fmesh/contour/polytree.h"
#include "fmesh/generate/polyfiller.h"
#include "fmesh/generate/triangularization.h"

namespace fmesh
{
	class GeneratorImpl
	{
	public:
		GeneratorImpl();
		virtual ~GeneratorImpl();

		trimesh::TriMesh* build(ClipperLib::Paths* paths, const GenParam& param, const Args& args);

		virtual void build() = 0;
	protected:
		void addPatch(Patch* patch, bool invert = false);
		void addPatches(std::vector<Patch*>& patches, bool invert = false);

		trimesh::TriMesh* generateFromPatches();
		void releaseResources();

		void _fillPolyTree(ClipperLib::PolyTree* tree, bool invert = false);
		void _fillPolyTreeOutline(ClipperLib::PolyTree* tree, bool invert = false);
		void _fillPolyTreeInner(ClipperLib::PolyTree* tree, bool invert = false);
		void _buildFromSamePolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag = 0);  //0 all, 1 outer, 2 inner
		void _buildFromDiffPolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag = 0);

		void _buildFromDiffPolyTree_drum(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag, ClipperLib::PolyTree& out);

		void _buildFromDiffPolyTree_firstLayer(ClipperLib::PolyTree* treeLower, int flag = 0);
		void _buildRoof(ClipperLib::PolyTree* polyTree, double roofHeight, double thickness);
	protected:
		ClipperLib::Paths* m_paths;
		ClipperLib::PolyTree m_poly;

		GenParam m_param;
		std::vector<std::string> m_args;

		std::vector<Patch*> m_patches;
	};
}

#endif // FMESH_GENERATORIMPL_1604466979729_H