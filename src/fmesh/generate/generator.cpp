#include "generator.h"
#include "fmesh/contour/contour.h"
#include "fmesh/generate/generatorimpl.h"
#include "fmesh/contour/path.h"

#include "fmesh/generate/simplegenerator.h"
#include "fmesh/generate/bottomgenerator.h"
#include "fmesh/generate/stepsgenerator.h"
#include "fmesh/generate/drumgenerator.h"
#include "fmesh/generate/italicsgenerator.h"
#include "fmesh/generate/slopegenerator.h"
#include "fmesh/generate/drumedgegenerator.h"
#include "fmesh/generate/sharpsidegenerator.h"
#include "fmesh/generate/slopebackgenerator.h"
#include "fmesh/generate/layergenerator.h"
#include "fmesh/generate/sharptopgenerator.h"
#include "fmesh/generate/reitalicsgenerator.h"
#include "fmesh/generate/drumedgedoublegenerator.h"
#include "fmesh/generate/sharpsidedoublegenerator.h"
#include "fmesh/generate/wideninnergenerator.h"
#include "fmesh/generate/widenoutergenerator.h"
#include "fmesh/generate/neongenerator.h"
#include "fmesh/generate/punchinggenerator.h"
#include "fmesh/generate/lampgenerator.h"
#include "fmesh/generate/nestedgenerator.h"

#include "mmesh/trimesh/trimeshutil.h"
#include <memory>
namespace fmesh
{
#define REGISTER(x, y)  { GeneratorIterator it = impls.find(x); \
						if(it != impls.end()) \
						{ \
							printf("register error."); \
						} else {\
							 GeneratorImpl* impl = new y();\
							impls.insert(GeneratorValue(x, impl)); }}

	void createBuildImpls(GeneratorImplMap& impls)
	{
		REGISTER("simple", SimpleGenerator)
		REGISTER("bottom", BottomGenerator)
		REGISTER("steps", StepsGenerator)
		REGISTER("drum", DrumGenerator)
		REGISTER("italics", ItalicsGenerator)
		REGISTER("slope", SlopeGenerator)
		REGISTER("drumedge", DrumedgeGenerator)
		REGISTER("sharpside", SharpsideGenerator)
		REGISTER("slopeback", SlopebackGenerator)
		REGISTER("layer", LayerGenerator)
		REGISTER("sharptop", SharptopGenerator)
		REGISTER("reitalics", ReItalicsGenerator)
		REGISTER("doublec", DrumedgeDoubleGenerator)
		REGISTER("doubles", SharpsideDoubleGenerator)
		REGISTER("inner", WidenInnerGenerator)
		REGISTER("outer", WidenOuterGenerator)
	}

	void destroyBuildImpls(GeneratorImplMap& impls)
	{
		for (GeneratorIterator it = impls.begin(); it != impls.end(); ++it)
			delete it->second;
		impls.clear();
	}

	Generator::Generator()
		:m_polyTree(nullptr)
	{
		createBuildImpls(m_generateImpls);
	}

	Generator::~Generator()
	{
		destroyBuildImpls(m_generateImpls);
	}

	ClipperLib::Paths* Generator::paths()
	{
		return m_paths.get();
	}

	void Generator::setPaths(ClipperLib::Paths* paths)
	{
		m_paths.reset(paths);

		ClipperLib::IntPoint bmin;
		ClipperLib::IntPoint bmax;
		fmesh::calculatePathBox(paths, bmin, bmax);
		//scale
		trimesh::vec3 bMin = trimesh::vec3(INT2MM(bmin.X), INT2MM(bmin.Y), INT2MM(bmin.Z));
		trimesh::vec3 bMax = trimesh::vec3(INT2MM(bmax.X), INT2MM(bmax.Y), INT2MM(bmax.Z));

		m_polyTree.reset();
	} 

	ClipperLib::PolyTree* Generator::polyTree()
	{
		if (!m_polyTree)
		{
			m_polyTree.reset(fmesh::convertPaths2PolyTree(m_paths.get()));
		}

		return m_polyTree.get();
	}

	GeneratorImpl* Generator::findImpl(const std::string& method)
	{
		GeneratorImpl* impl = nullptr;
		GeneratorIterator it = m_generateImpls.find(method);
		if (it != m_generateImpls.end())
			impl = it->second;
		return impl;
	}

	trimesh::TriMesh* Generator::build(const std::string& method, std::vector<std::string>& args)
	{
		ClipperLib::Paths* paths = m_paths.get();
		if (!paths)
			return nullptr;
		
		GeneratorImpl* impl = findImpl(method);
		trimesh::TriMesh* mesh = impl ? impl->generate(paths, ADParam()) : nullptr;
		return mesh;
	}

	GeneratorImpl* createGenerator(const ADParam& param)
	{
		GeneratorImpl* impl = nullptr;
		switch (param.shape_type)
		{
		case ADShapeType::adst_none:
			impl = new SimpleGenerator();
			break;
		case ADShapeType::adst_xiebian_top:
			impl = new ItalicsGenerator();
			break;
		case ADShapeType::adst_xiebian_bottom:
			impl = new ReItalicsGenerator();
			break;
		case ADShapeType::adst_gubian:
			impl = new DrumedgeGenerator();
			break;
		case ADShapeType::adst_cemianjianjiao:
			impl = new SharpsideGenerator();
			break;
		case ADShapeType::adst_yuanding:
			impl = new DrumGenerator();
			break;
		case ADShapeType::adst_jianjiao:
			impl = new SharptopGenerator();
			break;
		case ADShapeType::adst_xiemian_front:
			impl = new SlopeGenerator();
			break;
		case ADShapeType::adst_xiemian_back:
			impl = new SlopebackGenerator();
			break;
		case ADShapeType::adst_dingmianjieti:
			impl = new StepsGenerator();
			break;
		case ADShapeType::adst_shuangc:
			impl = new DrumedgeDoubleGenerator();
			break;
		case ADShapeType::adst_shuangjianjiao:
			impl = new SharpsideDoubleGenerator();
			break;
		case ADShapeType::adst_baobianzi_inner:
			impl = new WidenInnerGenerator();
			break;
		case ADShapeType::adst_baobianzi_outer:
			impl = new WidenOuterGenerator();
			break;
		case ADShapeType::adst_nihongdeng:
			impl = new NeonGenerator();
			break;
		case ADShapeType::adst_chongkong:
			impl = new PunchingGenerator();
			break;
		case ADShapeType::adst_loukong:
			break;
		case ADShapeType::adst_qiantao:
			impl = new NestedGenerator();
			break;
		case ADShapeType::adst_dengpao:
			impl = new LampGenerator();
			break;
		default:
			impl = new SimpleGenerator();
			break;
		}
		return impl;
	}

	GeneratorProxy::GeneratorProxy()
	{

	}

	GeneratorProxy::~GeneratorProxy()
	{

	}

	trimesh::TriMesh* GeneratorProxy::build(const ADParam& param, ClipperLib::Paths* paths,
		ExportParam* exportParam, ClipperLib::PolyTree* topTree, ClipperLib::PolyTree* bottomTree, bool buildShell)
	{
		if (!paths)
			return nullptr;

		std::unique_ptr<GeneratorImpl> impl(createGenerator(param));
		trimesh::TriMesh* mesh = nullptr;
		if (impl)
		{
			mesh = buildShell ? impl->generateShell(paths, param, exportParam, topTree, bottomTree)
				: impl->generate(paths, param);
		}
		return mesh;
	}

	void GeneratorProxy::setup(const ADParam& param, ClipperLib::Paths* paths, mmesh::StatusTracer* tracer)
	{
		if (!paths)
			return;

		m_impl.reset(createGenerator(param));
		if (m_impl)
			m_impl->setup(param, paths, tracer);
	}

	trimesh::TriMesh* GeneratorProxy::build()
	{
		return m_impl ? m_impl->generate() : nullptr;
	}

	trimesh::TriMesh* GeneratorProxy::buildShell()
	{
		return m_impl ? m_impl->generateShell() : nullptr;
	}

	void GeneratorProxy::buildBoard(const ExportParam& param, ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree)
	{
		if (m_impl)
			m_impl->generateBoard(param, topTree, bottomTree);
	}

	trimesh::TriMesh* GeneratorProxy::buildOmp(const ADParam& param, ClipperLib::Paths* paths,
		ExportParam* exportParam, ClipperLib::PolyTree* topTree, ClipperLib::PolyTree* bottomTree)
	{
		if (!paths)
			return nullptr;

		ClipperLib::PolyTree tree;
		fmesh::convertPaths2PolyTree(paths, tree);

		std::vector<ClipperLib::Paths> children;
		fmesh::split(tree, children);

		size_t size = children.size();
		if (size == 0)
			return nullptr;

		trimesh::TriMesh* mesh = new trimesh::TriMesh();
		std::vector<trimesh::TriMesh*> meshes(size);
#pragma omp parallel for
		for (int i = 0; i < size; i++)
		{
			std::unique_ptr<GeneratorImpl> impl(createGenerator(param));
			meshes.at(i) = impl ? impl->generate(&children.at(i), param) : nullptr;
		}

		mmesh::mergeTriMesh(mesh, meshes);
		for (trimesh::TriMesh* m : meshes)
			if(m)
				delete m;
		return mesh;
	}
}