#include "builder.h"
#include "trimesh2/TriMesh_algo.h"
#include "mmesh/trimesh/trimeshutil.h"
#include "mmesh/clipper/circurlar.h"

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
	//destroyBuildImpls(m_buildImpls);
}

ClipperLib::Paths* Builder::paths()
{
	return m_paths.get();
}

void Builder::setPaths(ClipperLib::Paths* paths)
{
	m_paths.reset(new ClipperLib::Paths());
	*m_paths = *paths;

	ClipperLib::IntPoint bmin(99999999, 99999999, 99999999);
	ClipperLib::IntPoint bmax(-99999999, -99999999, -99999999);
	for (ClipperLib::Path& path : *paths)
	{
		for (ClipperLib::IntPoint& p : path)
		{
			bmin.X = std::min(bmin.X, p.X);
			bmin.Y = std::min(bmin.Y, p.Y);
			bmin.Z = std::min(bmin.Z, p.Z);
			bmax.X = std::max(bmax.X, p.X);
			bmax.Y = std::max(bmax.Y, p.Y);
			bmax.Z = std::min(bmax.Z, p.Z);
		}
	}
	//scale
	trimesh::vec3 bMin = trimesh::vec3(INT2MM(bmin.X), INT2MM(bmin.Y), INT2MM(bmin.Z));
	trimesh::vec3 bMax = trimesh::vec3(INT2MM(bmax.X), INT2MM(bmax.Y), INT2MM(bmax.Z));
	trimesh::vec3 bSize = bMax - bMin;
	float scale = m_param.expectLen / std::min(bSize.x, bSize.y);
	for (ClipperLib::Path& path : *m_paths)
	{
		for (ClipperLib::IntPoint& p : path)
		{
			ClipperLib::IntPoint ep;
			ep.X = (int)(scale * ((double)p.X - (double)bmin.X) + (double)bmin.X);
			ep.Y = (int)(scale * ((double)p.Y - (double)bmin.Y) + (double)bmin.Y);

			p = ep;
		}
	}

	m_param.dmin = trimesh::vec2(bMin);
	m_param.dmax = trimesh::vec2(bMin) + trimesh::vec2(m_param.expectLen, m_param.expectLen);

	m_polyTree.reset();
	m_extendPolyTree.reset();
}

ClipperLib::PolyTree* Builder::polyTree()
{
	if (!m_polyTree)
	{
		m_polyTree.reset(fmesh::convertPaths2PolyTree(m_paths.get()));
	}

	return m_polyTree.get();
}

ClipperLib::PolyTree* Builder::extendPolyTree()
{
	if (!m_extendPolyTree)
	{
		m_extendPolyTree.reset(fmesh::offsetPolyTree(polyTree(), m_param.thickness / 2.0f));
	}

	return m_extendPolyTree.get();
}

ClipperLib::PolyTree* Builder::oddevenPolyTree()
{
	if (!m_extendPolyTree)
	{
		ClipperLib::PolyTree* tree = fmesh::offsetPolyTree(polyTree(), 5.0);
		m_extendPolyTree.reset(fmesh::extendPolyTree2PolyTree(tree, 0.5));
		delete tree;
	}

	return m_extendPolyTree.get();
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

	if (mesh)
	{
		trimesh::vec2 d = m_param.dmax - m_param.dmin;
		mesh->need_bbox();
		trimesh::trans(mesh, trimesh::vec3(d.x + 1.0f, 0.0f, 0.0f));
	}
	return mesh;
}



