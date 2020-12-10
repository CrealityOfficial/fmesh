#include "nestbuilder.h"
#include "fmesh/generate/simplegenerator.h"
#include "fmesh/generate/slotgenerator.h"

#include "mmesh/trimesh/trimeshutil.h"

namespace fmesh
{
	trimesh::TriMesh* nestBuild(ClipperLib::Paths* paths)
	{
		SlotGenerator generator1;
		SimpleGenerator generator2;
		ADParam param1, param2;
		param1.extend_width = 2.0f;
		param1.bottom_type = ADBottomType::adbt_close;
		param1.bottom_height = 3;
		param1.bottom_extend_width = 0.5f;
		param1.bottom_offset = 0;

		param2.extend_width = 0.5f;
		param2.bottom_offset = 4.0f;
		param2.bottom_type = ADBottomType::adbt_close;
		float gap = 0.1f;
		ClipperLib::Paths outer, inner;

		ClipperLib::ClipperOffset offset;
		offset.AddPaths(*paths, ClipperLib::jtMiter, ClipperLib::EndType::etClosedPolygon);
		offset.Execute(outer, (param1.extend_width / 2.0f) * 1000.0);
		offset.Execute(inner, -(param2.extend_width / 2.0f + gap) * 1000.0);

		trimesh::TriMesh* mesh1 = generator1.generate(&outer, param1);
		trimesh::TriMesh* mesh2 = generator2.generate(&inner, param2);

		trimesh::TriMesh* mesh = new trimesh::TriMesh();
		std::vector<trimesh::TriMesh*> meshes;
		meshes.push_back(mesh1);
		meshes.push_back(mesh2);
		mmesh::mergeTriMesh(mesh, meshes);

		delete mesh1;
		delete mesh2;
		return mesh;
	}
}