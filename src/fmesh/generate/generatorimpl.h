#ifndef FMESH_GENERATORIMPL_1604466979729_H
#define FMESH_GENERATORIMPL_1604466979729_H
#include <clipper/clipper.hpp>
#include "trimesh2/TriMesh.h"
#include <map>

#include "fmesh/common/export.h"
#include "fmesh/common/param.h"
#include "fmesh/generate/patch.h"
#include "fmesh/contour/polytree.h"
#include "fmesh/generate/polyfiller.h"
#include "fmesh/generate/triangularization.h"
#include "mmesh/common/selfdeffunc.h"

namespace fmesh
{
	class FMESH_API GeneratorImpl
	{
	public:
		GeneratorImpl();
		virtual ~GeneratorImpl();
		trimesh::TriMesh* generate(ClipperLib::Paths* paths, const ADParam& param);
		trimesh::TriMesh* generateShell(ClipperLib::Paths* paths, const ADParam& param,
			ExportParam* exportParam = nullptr, ClipperLib::PolyTree* topTree = nullptr, ClipperLib::PolyTree* bottomTree = nullptr);
		trimesh::TriMesh* generate(ClipperLib::PolyTree* tree, const ADParam& param,
			ExportParam* exportParam = nullptr, ClipperLib::PolyTree* topTree = nullptr, ClipperLib::PolyTree* bottomTree = nullptr);

		virtual void build() = 0;
		virtual void buildShell() {}
		virtual void buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree) {}

		void setup(const ADParam& param, ClipperLib::Paths* paths, mmesh::StatusTracer* tracer = nullptr);
		trimesh::TriMesh* generate();
		trimesh::TriMesh* generateShell();
		void generateBoard(const ExportParam& param, ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree);
	protected:
		void addPatch(Patch* patch, bool invert = false);
		void addPatches(std::vector<Patch*>& patches, bool invert = false);

		trimesh::TriMesh* generateFromPatches();
		void releaseResources();

		void _fillPolyTree(ClipperLib::PolyTree* tree, bool invert = false);
		void _fillPolyTreeInner(ClipperLib::PolyTree* tree, bool invert = false);//top bottom :close
		void _fillPolyTreeDepth14(ClipperLib::PolyTree* tree, bool invert = false);//top bottom :close
		void _buildFromSamePolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag = 0);  //0 all, 1 outer, 2 inner
		void _buildFromDiffPolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag = 0);

		void _fillPolyTreeReverseInner(ClipperLib::PolyTree* tree, bool invert = false);//?
		void _fillPolyTreeReverse(ClipperLib::PolyTree* tree, bool invert = false);
		void _fillPolyTreeReverseInnerNew(ClipperLib::PolyTree* tree, bool invert = false);
		void _fillPolyTreeSameZ(ClipperLib::PolyTree* tree, bool invert = false);  //steps
		void _dealPolyTreeAxisZ(ClipperLib::PolyTree* tree, double slope, double min, double height=0); //slope

		void _buildFromDiffPolyTree_diffSafty(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta = 1.0, int flag = 0, bool invert = false,bool invertXor=false);
		void _buildFromDiffPolyTree_xor(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta = 1.0, int flag = 0, bool invert = false); //steps
		void _buildFromDiffPolyTree_Inner(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta = 1.0, int flag = 0, bool invert = false); //steps	
		void _buildFromDiffPolyTree_onePoly(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta = 1.0, int flag = 0, bool invert = false);

		void _buildFromDiffPolyTree_all(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, double delta = 1.0, int flag = 0, bool invertInner = false, bool invertOuter = false);
		void _buildFromDiffPolyTree_all_same(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp,double delta, int flag = 0, bool invert = false);

		void _buileMergePloyTree(ClipperLib::PolyTree* treeOuter, ClipperLib::PolyTree* treeInner, ClipperLib::PolyTree& treeNew);
		void _buileDetachPloyTree(ClipperLib::PolyTree* tree, ClipperLib::PolyTree& treeNew,double delta,int flag = 0);//0 all 2 inner 3 outer
		void _buildFromDiffPolyTree_firstLayer(ClipperLib::PolyTree* treeLower, int flag = 0);

		void _buildTop(ClipperLib::PolyTree& treeTop, double& hTop,double offset = 0);
		void _buildBottom(ClipperLib::PolyTree& treeBottom, double& hBottom, double offset = 0);
		void _buildTopBottom(ClipperLib::PolyTree* treeBottom, ClipperLib::PolyTree* treeTop, double offsetB = 0, double offsetT = 0);

		void _buildTopBottom_onepoly(ClipperLib::PolyTree* treeBottom, ClipperLib::PolyTree* treeTop, double offsetB = 0, double offsetT = 0);
		void _buildTop_onepoly(ClipperLib::PolyTree& treeTop, double& hTop, double offset = 0);
		void _buildBottom_onepoly(ClipperLib::PolyTree& treeBottom, double& hBottom, double offset = 0);

		void _simplifyPoly(ClipperLib::PolyTree* poly, double distance=0);
		void _simplifyPolyOneploy(ClipperLib::PolyTree* poly, bool outer=true, double distance = 0);

		void areaPoly(ClipperLib::PolyTree& poly, std::vector<double>& area);
	protected:
		ClipperLib::Paths* m_paths;
		ClipperLib::PolyTree m_poly;

		ADParam m_adParam;
		trimesh::vec2 dmin;
		trimesh::vec2 dmax;

		ExportParam m_exportParam;
		ClipperLib::PolyTree* m_topTree;
		ClipperLib::PolyTree* m_bottomTree;

		std::vector<std::string> m_args;

		std::vector<Patch*> m_patches;

		mmesh::StatusTracer* m_tracer;
	};
}

#endif // FMESH_GENERATORIMPL_1604466979729_H