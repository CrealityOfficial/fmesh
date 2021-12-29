#include "optimize.h"
#include "fmesh/contour/polytree.h"
#include "fmesh/generate/generator.h"
#include "fmesh/generate/generatorimpl.h"
#include "mmesh/trimesh/trimeshutil.h"

#include "fmesh/generate/simplegenerator.h"
namespace fmesh
{
	typedef std::function<trimesh::TriMesh* (const ADParam&, ClipperLib::Paths*)> buildFunc;

		//adst_none,
		//adst_xiebian_top,
		//adst_xiebian_bottom,
		//adst_gubian,
		//adst_cemianjianjiao,
		//adst_yuanding,
		//adst_jianjiao,
		//adst_xiemian_front,
		//adst_xiemian_back,
		//adst_dingmianjieti,

	trimesh::TriMesh* build_adst_none(const ADParam& param, ClipperLib::Paths* paths)
	{
		SimpleGenerator generator;
		return generator.generate(paths, param);
	}

	buildFunc ffuncs[(unsigned long)ADShapeType::adst_num] = {
		build_adst_none,
		build_adst_none,
		build_adst_none,
		build_adst_none,
		build_adst_none,
		build_adst_none,
		build_adst_none,
		build_adst_none,
		build_adst_none,
		build_adst_none
	};

	trimesh::TriMesh* buildOptimize(const ADParam& param, ClipperLib::Paths* paths, OptTracer* tracer)
	{
		if (!paths)
			return nullptr;

		if (tracer)
			tracer->start();

		ClipperLib::PolyTree tree;
		fmesh::convertPaths2PolyTree(paths, tree);

		std::vector<ClipperLib::Paths> children;
		fmesh::split_omp(tree, children);

		size_t size = children.size();
		if (size == 0)
			return nullptr;

		if (tracer)
			tracer->splitOver();

		trimesh::TriMesh* mesh = new trimesh::TriMesh();
		std::vector<trimesh::TriMesh*> meshes(size);
		buildFunc func = ffuncs[(int)param.shape_type];
#pragma omp parallel for
		for (int i = 0; i < size; i++)
			meshes.at(i) = func(param, &children.at(i));

		if (tracer)
			tracer->buildOver();

		mmesh::mergeTriMesh(mesh, meshes);
		for (trimesh::TriMesh* m : meshes)
			if (m)
				delete m;

		if (tracer)
			tracer->mergeOver();

		return mesh;
	}
}