#ifndef FMESH_GENERATOR_1604462758368_H
#define FMESH_GENERATOR_1604462758368_H
#include "clipperxyz/clipper.hpp"
#include "trimesh2/TriMesh.h"
#include "fmesh/common/export.h"
#include "fmesh/common/param.h"

#include <map>
#include <string>

namespace mmesh
{
	class StatusTracer;
}
namespace fmesh
{
	class GeneratorImpl;

	typedef std::map<std::string, GeneratorImpl*> GeneratorImplMap;
	typedef GeneratorImplMap::iterator GeneratorIterator;
	typedef GeneratorImplMap::value_type GeneratorValue;

	void createBuildImpls(GeneratorImplMap& impls);
	void destroyBuildImpls(GeneratorImplMap& impls);

	class FMESH_API Generator
	{
	public:
		Generator();
		virtual ~Generator();

		void setPaths(ClipperLibXYZ::Paths* paths);
		ClipperLibXYZ::Paths* paths();

		ClipperLibXYZ::PolyTree* polyTree();
		trimesh::TriMesh* build(const std::string& method, std::vector<std::string>& args);
	protected:
		GeneratorImpl* findImpl(const std::string& method);
	protected:
		std::unique_ptr<ClipperLibXYZ::Paths> m_paths;
		std::unique_ptr<ClipperLibXYZ::PolyTree> m_polyTree;
		
		std::map<std::string, GeneratorImpl*> m_generateImpls;
	};

	GeneratorImpl* createGenerator(const ADParam& param);
	class FMESH_API GeneratorProxy
	{
	public:
		GeneratorProxy();
		virtual ~GeneratorProxy();

		trimesh::TriMesh* build(const ADParam& param, ClipperLibXYZ::Paths* paths, 
			ExportParam* exportParam = nullptr, ClipperLibXYZ::PolyTree* topTree = nullptr, ClipperLibXYZ::PolyTree* bottomTree = nullptr, bool buildShell = false);

		trimesh::TriMesh* buildOmp(const ADParam& param, ClipperLibXYZ::Paths* paths,
			ExportParam* exportParam = nullptr, ClipperLibXYZ::PolyTree* topTree = nullptr, ClipperLibXYZ::PolyTree* bottomTree = nullptr);

		//seperate
		void setup(const ADParam& param, ClipperLibXYZ::Paths* paths, mmesh::StatusTracer* tracer = nullptr);
		trimesh::TriMesh* build();
		trimesh::TriMesh* buildShell();
		void buildBoard(const ExportParam& param, ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree);
	protected:
		std::unique_ptr<GeneratorImpl> m_impl;
	};
}

#endif // FMESH_GENERATOR_1604462758368_H