#ifndef FMESH_GENERATORIMPL_1604466979729_H
#define FMESH_GENERATORIMPL_1604466979729_H
#include "clipperxyz/clipper.hpp"
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
		trimesh::TriMesh* generate(ClipperLibXYZ::Paths* paths, const ADParam& param);
		trimesh::TriMesh* generateShell(ClipperLibXYZ::Paths* paths, const ADParam& param,
			ExportParam* exportParam = nullptr, ClipperLibXYZ::PolyTree* topTree = nullptr, ClipperLibXYZ::PolyTree* bottomTree = nullptr);
		trimesh::TriMesh* generate(ClipperLibXYZ::PolyTree* tree, const ADParam& param,
			ExportParam* exportParam = nullptr, ClipperLibXYZ::PolyTree* topTree = nullptr, ClipperLibXYZ::PolyTree* bottomTree = nullptr);

		virtual void build() = 0;
		virtual void buildShell() {}
		virtual void buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree) {}

		void setup(const ADParam& param, ClipperLibXYZ::Paths* paths, mmesh::StatusTracer* tracer = nullptr);
		trimesh::TriMesh* generate();
		trimesh::TriMesh* generateShell();
		void generateBoard(const ExportParam& param, ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree);
	protected:
		void addPatch(Patch* patch, bool invert = false);
		void addPatches(std::vector<Patch*>& patches, bool invert = false);

		trimesh::TriMesh* generateFromPatches();
		void releaseResources();

		void _fillPolyTree(ClipperLibXYZ::PolyTree* tree, bool invert = false);
		void _fillPolyTreeInner(ClipperLibXYZ::PolyTree* tree, bool invert = false);//top bottom :close
		void _fillPolyTreeDepth14(ClipperLibXYZ::PolyTree* tree, bool invert = false);//top bottom :close
		void _fillPolyTreeDepth23(ClipperLibXYZ::PolyTree* tree, bool invert = false);//top bottom :close
		void _buildFromSamePolyTree(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, int flag = 0);  //0 all, 1 outer, 2 inner
		void _buildFromDiffPolyTree(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, int flag = 0);
		void _fillPolyTreeOnePloy(ClipperLibXYZ::PolyTree* tree, bool invert = false);

		void _fillPolyTreeReverseInner(ClipperLibXYZ::PolyTree* tree, bool invert = false);//?
		void _fillPolyTreeReverse(ClipperLibXYZ::PolyTree* tree, bool invert = false);
		void _fillPolyTreeReverseInnerNew(ClipperLibXYZ::PolyTree* tree, bool invert = false);
		void _fillPolyTreeSameZ(ClipperLibXYZ::PolyTree* tree, bool invert = false);  //steps
		void _dealPolyTreeAxisZ(ClipperLibXYZ::PolyTree* tree, double slope, double min, double height=0); //slope

		void _buildFromDiffPolyTree_diffSafty(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta = 1.0, int flag = 0, bool invert = false,bool invertXor=false);
		void _buildFromDiffPolyTree_diff(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta = 1.0, int flag = 0, bool invert = false);
		void _buildFromDiffPolyTree_xor(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta = 1.0, int flag = 0, bool invert = false); //steps
		void _buildFromDiffPolyTree_Inner(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta = 1.0, int flag = 0, bool invert = false); //steps	
		void _buildFromDiffPolyTree_onePoly(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta = 1.0, int flag = 0, bool invert = false);

		void _buildFromDiffPolyTree_all(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, double delta = 1.0, int flag = 0, bool invertInner = false, bool invertOuter = false);
		void _buildFromDiffPolyTree_all_same(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp,double delta, int flag = 0, bool invert = false);

		void _buileMergePloyTree(ClipperLibXYZ::PolyTree* treeOuter, ClipperLibXYZ::PolyTree* treeInner, ClipperLibXYZ::PolyTree& treeNew);
		void _buileDetachPloyTree(ClipperLibXYZ::PolyTree* tree, ClipperLibXYZ::PolyTree& treeNew,double delta,int flag = 0);//0 all 2 inner 3 outer
		void _buildFromDiffPolyTree_firstLayer(ClipperLibXYZ::PolyTree* treeLower, int flag = 0);

		void _buildTop(ClipperLibXYZ::PolyTree& treeTop, double& hTop,double offset = 0);
		void _buildBottom(ClipperLibXYZ::PolyTree& treeBottom, double& hBottom, double offset = 0);
		void _buildTopBottom(ClipperLibXYZ::PolyTree* treeBottom, ClipperLibXYZ::PolyTree* treeTop, double offsetB = 0, double offsetT = 0);
		void _buildTopBottomDiff(ClipperLibXYZ::PolyTree* treeBottom, ClipperLibXYZ::PolyTree* treeTop, double offsetB = 0, double offsetT = 0);
		void _buildTopDiff(ClipperLibXYZ::PolyTree& treeTop, ClipperLibXYZ::PolyTree* top);
		void _buildBottomDiff(ClipperLibXYZ::PolyTree& treeBottom, ClipperLibXYZ::PolyTree* bottom);

		void _buildTopBottom_onepoly(ClipperLibXYZ::PolyTree* treeBottom, ClipperLibXYZ::PolyTree* treeTop, double offsetB = 0, double offsetT = 0);
		void _buildTop_onepoly(ClipperLibXYZ::PolyTree& treeTop, double& hTop, double offset = 0);
		void _buildBottom_onepoly(ClipperLibXYZ::PolyTree& treeBottom, double& hBottom, double offset = 0);

		void _simplifyPoly(ClipperLibXYZ::PolyTree* poly, double distance=0);
		void _simplifyPolyOneploy(ClipperLibXYZ::PolyTree* poly, bool outer=true, double distance = 0);

		void areaPoly(ClipperLibXYZ::PolyTree& poly, std::vector<double>& area);
	protected:
		ClipperLibXYZ::Paths* m_paths;
		ClipperLibXYZ::PolyTree m_poly;

		ADParam m_adParam;
		trimesh::vec2 dmin;
		trimesh::vec2 dmax;

		ExportParam m_exportParam;
		ClipperLibXYZ::PolyTree* m_topTree;
		ClipperLibXYZ::PolyTree* m_bottomTree;

		std::vector<std::string> m_args;

		std::vector<Patch*> m_patches;

		mmesh::StatusTracer* m_tracer;
	};
}

#endif // FMESH_GENERATORIMPL_1604466979729_H