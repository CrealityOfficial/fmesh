#ifndef FMESH_GENERATOR_1604462758368_H
#define FMESH_GENERATOR_1604462758368_H
#include <clipper/clipper.hpp>
#include "trimesh2/TriMesh.h"
#include "fmesh/common/param.h"

#include <map>
#include <string>

namespace fmesh
{
	class GeneratorImpl;

	typedef std::map<std::string, GeneratorImpl*> GeneratorImplMap;
	typedef GeneratorImplMap::iterator GeneratorIterator;
	typedef GeneratorImplMap::value_type GeneratorValue;

	void createBuildImpls(GeneratorImplMap& impls);
	void destroyBuildImpls(GeneratorImplMap& impls);

	class Generator
	{
	public:
		Generator();
		virtual ~Generator();

		void setPaths(ClipperLib::Paths* paths);
		void setParam(const GenParam& param);
		ClipperLib::Paths* paths();

		ClipperLib::PolyTree* polyTree();
		trimesh::TriMesh* build(const std::string& method, std::vector<std::string>& args);
	protected:
		GeneratorImpl* findImpl(const std::string& method);
	protected:
		std::unique_ptr<ClipperLib::Paths> m_paths;
		std::unique_ptr<ClipperLib::PolyTree> m_polyTree;
		
		std::map<std::string, GeneratorImpl*> m_generateImpls;
		GenParam m_param;
		F2MParam m_modelparam;
	};
}

#endif // FMESH_GENERATOR_1604462758368_H