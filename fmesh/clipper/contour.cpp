#include "contour.h"

namespace fmesh
{
	void fill(trimesh::vec3* vertex, ClipperLibXYZ::Path& path)
	{
		size_t size = path.size();
		for (size_t i = 0; i < size; ++i)
		{
			ClipperLibXYZ::IntPoint& p = path.at(i);
			trimesh::vec3* data = vertex + i;
			*data = trimesh::vec3(INT2MM(p.X), INT2MM(p.Y), 0.0f);
		}
	}
}