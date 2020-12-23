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

		trimesh::TriMesh* build(ClipperLib::Paths* paths, const GenParam& param, const F2MParam& modelparam, const Args& args);
		trimesh::TriMesh* generate(ClipperLib::Paths* paths, const ADParam& param);

		virtual void build() = 0;
	protected:
		void addPatch(Patch* patch, bool invert = false);
		void addPatches(std::vector<Patch*>& patches, bool invert = false);

		trimesh::TriMesh* generateFromPatches();
		void releaseResources();

		void _fillPolyTree(ClipperLib::PolyTree* tree, bool invert = false);
		void _fillPolyTreeOutline(ClipperLib::PolyTree* tree, bool invert = false);
		void _fillPolyTreeInner(ClipperLib::PolyTree* tree, bool invert = false);
		void _fillPolyTreeDepth14(ClipperLib::PolyTree* tree, bool invert = false);
		void _buildFromSamePolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag = 0);  //0 all, 1 outer, 2 inner
		void _buildFromDiffPolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag = 0);

		void _buildFromDiffPolyTree_drum(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag, ClipperLib::PolyTree& out);
		void _fillPolyTreeReverseInner(ClipperLib::PolyTree* tree, bool invert = false);
		void _dealPolyTreeAxisZ(ClipperLib::PolyTree* tree, double slope, double height=0);
		void _buildFromDiffPolyTree_diff(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag = 0);

		void _buildFromDiffPolyTree_firstLayer(ClipperLib::PolyTree* treeLower, int flag = 0);
		void _buildRoof(ClipperLib::PolyTree* polyTree, double roofHeight, double thickness);

		void _buildTop(ClipperLib::PolyTree& treeTop, double& hTop,double offset=0);
		void _buildBottom(ClipperLib::PolyTree& treeBottom, double& hBottom, double offset = 0);
		void _buildTopBottom(ClipperLib::PolyTree* treeBottom, ClipperLib::PolyTree* treeTop, double offsetB = 0, double offsetT = 0);
	protected:
		ClipperLib::Paths* m_paths;
		ClipperLib::PolyTree m_poly;

		GenParam m_param;
		F2MParam m_modelparam;

		ADParam m_adParam;
		trimesh::vec2 dmin;
		trimesh::vec2 dmax;

		std::vector<std::string> m_args;

		std::vector<Patch*> m_patches;
	};
}

#endif // FMESH_GENERATORIMPL_1604466979729_H