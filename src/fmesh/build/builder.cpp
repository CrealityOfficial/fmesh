#include "builder.h"
#include "mmesh/trimesh/trimeshutil.h"
#include "mmesh/clipper/circurlar.h"

#include "fmesh/contour/path.h"

#include "buildimpl.h"

#include <assert.h>

Builder::Builder()
	:m_polyTree(nullptr)
{
	createBuildImpls(m_buildImpls);

	m_param.expectLen = 50.0f;
	m_param.totalH = 15.0f;
	m_param.initH = 5.0f;
	m_param.thickness = 1.0f;
	m_param.bottomH = 5.0f;
}

Builder::~Builder()
{
	destroyBuildImpls(m_buildImpls);
}

ClipperLib::Paths* Builder::paths()
{
	return m_paths.get();
}

void Builder::setPaths(ClipperLib::Paths* paths)
{
	m_paths.reset(paths);

	ClipperLib::IntPoint bmin;
	ClipperLib::IntPoint bmax;
	fmesh::calculatePathBox(paths, bmin, bmax);
	//scale
	trimesh::vec3 bMin = trimesh::vec3(INT2MM(bmin.X), INT2MM(bmin.Y), INT2MM(bmin.Z));
	trimesh::vec3 bMax = trimesh::vec3(INT2MM(bmax.X), INT2MM(bmax.Y), INT2MM(bmax.Z));

	m_param.dmin = trimesh::vec2(bMin);
	m_param.dmax = trimesh::vec2(bMin);

	m_polyTree.reset();
}

ClipperLib::PolyTree* Builder::polyTree()
{
	if (!m_polyTree)
	{
		m_polyTree.reset(fmesh::convertPaths2PolyTree(m_paths.get()));
	}

	return m_polyTree.get();
}

BuildImpl* Builder::findImpl(const std::string& method)
{
	BuildImpl* impl = nullptr;
	BIMIterator it = m_buildImpls.find(method);
	if (it != m_buildImpls.end())
		impl = it->second;
	return impl;
}

trimesh::TriMesh* Builder::build(const std::string& method, std::vector<std::string>& args)
{
	ClipperLib::Paths* paths = m_paths.get();
	if (!paths)
		return nullptr;

	BuildImpl* impl = findImpl(method);
	m_param.args.swap(args);

	trimesh::TriMesh* mesh = impl ? impl->build(paths, m_param) : nullptr;
	return mesh;
}



