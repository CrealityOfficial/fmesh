#include "generator.h"
#include "fmesh/contour/contour.h"
#include "fmesh/generate/generatorimpl.h"
#include "fmesh/contour/path.h"

#include "fmesh/generate/simplegenerator.h"
#include "fmesh/generate/fillgenerator.h"
#include "fmesh/generate/bottomgenerator.h"
#include "fmesh/generate/frustumgenerator.h"
#include "fmesh/generate/stepsgenerator.h"
#include "fmesh/generate/exteriorgenerator.h"
#include "fmesh/generate/stepgenerator.h"
#include "fmesh/generate/roofgenerator.h"
#include "fmesh/generate/drumgenerator.h"
#include "fmesh/generate/italicsgenerator.h"
#include "fmesh/generate/slopegenerator.h"

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
		REGISTER("fill", FillGenerator)
		REGISTER("bottom", BottomGenerator)
		REGISTER("frustum", FrustumGenerator)
		REGISTER("steps", StepsGenerator)
		REGISTER("exterior", ExteriorGenerator)
		REGISTER("step", StepGenerator)
		REGISTER("roof", RoofGenerator)
		REGISTER("drum", DrumGenerator)
		REGISTER("italics", ItalicsGenerator)
		REGISTER("slope", SlopeGenerator)
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

	void Generator::setParam(const GenParam& param)
	{
		m_param = param;
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

		m_modelparam.dmin = trimesh::vec2(bMin);
		m_modelparam.dmax = trimesh::vec2(bMax);

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
		trimesh::TriMesh* mesh = impl ? impl->build(paths, m_param, m_modelparam,args) : nullptr;
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
		case ADShapeType::adst_xiebian_bottom:
		case ADShapeType::adst_gubian:
		case ADShapeType::adst_cemianjianjiao:
		case ADShapeType::adst_yuanding:
		case ADShapeType::adst_jianjiao:
		case ADShapeType::adst_xiemian_front:
		case ADShapeType::adst_xiemian_back:
		case ADShapeType::adst_dingmianjieti:
			impl = new SimpleGenerator();
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

	trimesh::TriMesh* GeneratorProxy::build(const ADParam& param, ClipperLib::Paths* paths)
	{
		if (!paths)
			return nullptr;

		std::unique_ptr<GeneratorImpl> impl(createGenerator(param));
		trimesh::TriMesh* mesh = impl ? impl->generate(paths, param) : nullptr;
		return mesh;
	}
}