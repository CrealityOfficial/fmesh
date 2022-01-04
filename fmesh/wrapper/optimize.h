#ifndef FMESH_OPTIMIZE_1610639694318_H
#define FMESH_OPTIMIZE_1610639694318_H
#include "fmesh/common/export.h"
#include "fmesh/common/param.h"
#include "trimesh2/TriMesh.h"
#include "clipperxyz/clipper.hpp"

namespace fmesh
{
	class OptTracer
	{
	public:
		virtual ~OptTracer() {}
		virtual void start() = 0;
		virtual void splitOver() = 0;
		virtual void buildOver() = 0;
		virtual void mergeOver() = 0;
	};

	FMESH_API trimesh::TriMesh* buildOptimize(const ADParam& param, ClipperLibXYZ::Paths* paths, OptTracer* tracer = nullptr);
}

#endif // FMESH_OPTIMIZE_1610639694318_H