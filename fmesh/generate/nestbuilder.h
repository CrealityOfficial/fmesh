#ifndef FMESH_NESTBUILDER_1607592705849_H
#define FMESH_NESTBUILDER_1607592705849_H
#include "fmesh/common/export.h"
#include "trimesh2/TriMesh.h"
#include "clipper/clipper.hpp"

namespace fmesh
{
	FMESH_API trimesh::TriMesh* nestBuild(ClipperLib::Paths* paths);
}

#endif // FMESH_NESTBUILDER_1607592705849_H