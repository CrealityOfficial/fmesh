#ifndef _BUILDER_1603113999404_H
#define _BUILDER_1603113999404_H
#include <clipper/clipper.hpp>
#include "trimesh2/TriMesh.h"

#include <map>
#include <string>

#include "fmesh/build/offsetfunc.h"

class BuildImpl;
class Builder
{
public:
	Builder();
	virtual ~Builder();

	void setPaths(ClipperLib::Paths* paths);
	ClipperLib::Paths* paths();

	ClipperLib::PolyTree* polyTree();
	ClipperLib::PolyTree* extendPolyTree();
	ClipperLib::PolyTree* oddevenPolyTree();

	trimesh::TriMesh* build(const std::string& method, std::vector<std::string>& args);
protected:
	BuildImpl* findImpl(const std::string& method);
protected:
	std::unique_ptr<ClipperLib::Paths> m_paths;
	std::unique_ptr<ClipperLib::PolyTree> m_polyTree;
	std::unique_ptr<ClipperLib::PolyTree> m_extendPolyTree;

	std::map<std::string, BuildImpl*> m_buildImpls;

	F2MParam m_param;
};

#endif // _BUILDER_1603113999404_H