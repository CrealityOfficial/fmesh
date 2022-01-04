#ifndef FMESH_NESTBUILDER_1607592705849_H
#define FMESH_NESTBUILDER_1607592705849_H
#include "fmesh/common/export.h"
#include "trimesh2/TriMesh.h"
#include "clipperxyz/clipper.hpp"

namespace fmesh
{
	FMESH_API trimesh::TriMesh* nestBuild(ClipperLibXYZ::Paths* paths);
}

#endif // FMESH_NESTBUILDER_1607592705849_H