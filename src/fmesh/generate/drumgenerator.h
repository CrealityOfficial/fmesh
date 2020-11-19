#ifndef FMESH_DRUMGENERATOR_1604649717545_H
#define FMESH_DRUMGENERATOR_1604649717545_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class DrumGenerator : public GeneratorImpl
	{
	public:
		DrumGenerator();
		virtual ~DrumGenerator();

		void build() override;

		void sort(std::vector<Patch*>& patches);

		float cross(float x1, float y1, float x2, float y2);

		float compare(trimesh::vec3 a, trimesh::vec3 b, trimesh::vec3 c);//¼ÆËã¼«½Ç

		bool cmp2(trimesh::vec3 a, trimesh::vec3 b);
	};
}

#endif // FMESH_DRUMGENERATOR_1604649717545_H