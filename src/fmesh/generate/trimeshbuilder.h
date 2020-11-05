#ifndef FMESH_TRIMESHBUILDER_1604539128885_H
#define FMESH_TRIMESHBUILDER_1604539128885_H
#include "trimesh2/TriMesh.h"
#include "fmesh/generate/patch.h"

namespace fmesh
{
	trimesh::TriMesh* buildFromPatches(std::vector<Patch*>& patches);
	trimesh::TriMesh* buildFromPatch(Patch* patch);
}

#endif // FMESH_TRIMESHBUILDER_1604539128885_H