#ifndef FMESH_TRIMESHBUILDER_1604539128885_H
#define FMESH_TRIMESHBUILDER_1604539128885_H
#include "fmesh/common/export.h"
#include "trimesh2/TriMesh.h"
#include "fmesh/generate/patch.h"

namespace fmesh
{
	FMESH_API trimesh::TriMesh* buildFromPatches(std::vector<Patch*>& patches);
	FMESH_API trimesh::TriMesh* buildFromPatch(Patch* patch);
}

#endif // FMESH_TRIMESHBUILDER_1604539128885_H