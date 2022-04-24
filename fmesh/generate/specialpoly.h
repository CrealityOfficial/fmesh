#ifndef FMESH_SPECIALPOLY_1604475054469_H
#define FMESH_SPECIALPOLY_1604475054469_H
#include "clipperxyz/clipper.hpp"
#include "fmesh/common/export.h"
#include "trimesh2/Vec.h"

namespace fmesh
{
	typedef struct SAABB
	{
		ClipperLibXYZ::IntPoint pMmin;
		ClipperLibXYZ::IntPoint pMax;
		SAABB()
			:pMmin((std::numeric_limits<ClipperLibXYZ::cInt>::max(), std::numeric_limits<ClipperLibXYZ::cInt>::max()))
			, pMax((std::numeric_limits<ClipperLibXYZ::cInt>::min(), std::numeric_limits<ClipperLibXYZ::cInt>::min()))
		{}
	}aabb;

	void FMESH_API generateLines(ClipperLibXYZ::Paths& originPloy, ClipperLibXYZ::Paths& newPaths, const double& laceGap, const double& laceRadius, bool isskeleton = false);
	void FMESH_API generateRounds(const ClipperLibXYZ::Paths& originPath, ClipperLibXYZ::Paths& newPloy, const double& laceGap, const double& laceRadius);

	bool isCollision(const ClipperLibXYZ::Paths& paths, const ClipperLibXYZ::IntPoint& point, const double& smallest_dist);

	inline double vSize2f(const ClipperLibXYZ::IntPoint& p0, const ClipperLibXYZ::IntPoint& p1)
	{
		return sqrt(pow((p1.X - p0.X), 2) + pow((p1.Y - p0.Y), 2)) / 1000.0;
	}

	int pathCount(ClipperLibXYZ::Path* path, const ClipperLibXYZ::IntPoint& p);
	bool deletePoint(ClipperLibXYZ::Path* path, int index);
	int findNext(ClipperLibXYZ::Path* path, ClipperLibXYZ::IntPoint point);
	void FMESH_API sortPath(ClipperLibXYZ::Path* path, ClipperLibXYZ::Paths* paths,bool getPerLine = false);

	float PointTOline(ClipperLibXYZ::IntPoint const& a, ClipperLibXYZ::IntPoint const& b, ClipperLibXYZ::IntPoint const& p);
	float getMinLen(const ClipperLibXYZ::Paths& paths, const ClipperLibXYZ::IntPoint point);
	float optimizePaths(ClipperLibXYZ::Paths& paths, ClipperLibXYZ::Paths& pathOrigin);

	aabb getAABB(ClipperLibXYZ::Path* path);
}

#endif // FMESH_SPECIALPOLY_1604475054469_H