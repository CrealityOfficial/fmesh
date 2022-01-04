#ifndef FMESH_CONTOUR_1610427389411_H
#define FMESH_CONTOUR_1610427389411_H
#include "fmesh/common/export.h"
#include "clipperxyz/clipper.hpp"
#include "trimesh2/Vec.h"

typedef std::vector<trimesh::dvec2> ContourPath;
typedef std::vector<ContourPath> ContourPaths;

namespace fmesh
{
	FMESH_API void convert(const ClipperLibXYZ::Paths& inPaths, ContourPaths& outPaths);
	FMESH_API void convert(const ClipperLibXYZ::Path& inPath, ContourPath& outPath);
	FMESH_API void convert(const ContourPaths& inPaths, ClipperLibXYZ::Paths& outPaths);
	FMESH_API void convert(const ContourPath& inPath, ClipperLibXYZ::Path& outPath);

	FMESH_API void calculateContourBox(ContourPaths* paths, trimesh::dvec3& outMin, trimesh::dvec3& outMax);
}

#endif // FMESH_CONTOUR_1610427389411_H