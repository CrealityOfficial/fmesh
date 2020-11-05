#include "generator.h"
#include "fmesh/contour/contour.h"
#include "fmesh/generate/generatorimpl.h"

#include "fmesh/generate/simplegenerator.h"
#include "fmesh/generate/fillgenerator.h"
#include "fmesh/generate/bottomgenerator.h"
#include "fmesh/generate/frustumgenerator.h"
#include "fmesh/generate/stepsgenerator.h"
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
		trimesh::TriMesh* mesh = impl ? impl->build(paths, m_param, args) : nullptr;
		return mesh;
	}
}