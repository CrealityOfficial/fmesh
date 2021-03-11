#ifndef FMESH_CONTOUR_1610427389411_H
#define FMESH_CONTOUR_1610427389411_H
#include "fmesh/common/export.h"
#include <clipper/clipper.hpp>
#include "trimesh2/Vec.h"

typedef std::vector<trimesh::dvec2> ContourPath;
typedef std::vector<ContourPath> ContourPaths;

namespace fmesh
{
	FMESH_API void convert(const ClipperLib::Paths& inPaths, ContourPaths& outPaths);
	FMESH_API void convert(const ClipperLib::Path& inPath, ContourPath& outPath);
	FMESH_API void convert(const ContourPaths& inPaths, ClipperLib::Paths& outPaths);
	FMESH_API void convert(const ContourPath& inPath, ClipperLib::Path& outPath);

	FMESH_API void calculateContourBox(ContourPaths* paths, trimesh::dvec3& outMin, trimesh::dvec3& outMax);
}

#endif // FMESH_CONTOUR_1610427389411_H