#include "trimeshbuilder.h"

namespace fmesh
{
	trimesh::TriMesh* buildFromPatches(std::vector<Patch*>& patches)
	{
		size_t triangleSize = 0;
		for (Patch* patch : patches)
			triangleSize += patch->size() / 3;
		if (triangleSize == 0)
			return nullptr;

		size_t vertexSize = 3 * triangleSize;
		trimesh::TriMesh* mesh = new trimesh::TriMesh();
		mesh->vertices.resize(vertexSize);
		mesh->faces.resize(triangleSize);

		int vertexIndex = 0;
		int triangleIndex = 0;
		for (Patch* patch : patches)
		{
			size_t size = patch->size() / 3;
			for (size_t i = 0; i < size; ++i)
			{
				trimesh::TriMesh::Face& f = mesh->faces.at(triangleIndex++);
				for (size_t j = 0; j < 3; ++j)
				{
					f[j] = vertexIndex;
					mesh->vertices.at(vertexIndex++) = patch->at(3 * i + j);
				}
			}
		}
		return mesh;
	}

	trimesh::TriMesh* buildFromPatch(Patch* patch)
	{
		std::vector<Patch*> patches;
		if(patch)
			patches.push_back(patch);
		return buildFromPatches(patches);
	}
}