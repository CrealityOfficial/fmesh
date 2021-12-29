#ifndef FMESH_SPECIALPOLY_1604475054469_H
#define FMESH_SPECIALPOLY_1604475054469_H
#include <clipper/clipper.hpp>
#include "fmesh/common/export.h"
#include "trimesh2/Vec.h"

namespace fmesh
{
	typedef struct SAABB
	{
		ClipperLib::IntPoint pMmin;
		ClipperLib::IntPoint pMax;
		SAABB()
			:pMmin((std::numeric_limits<ClipperLib::cInt>::max(), std::numeric_limits<ClipperLib::cInt>::max()))
			, pMax((std::numeric_limits<ClipperLib::cInt>::min(), std::numeric_limits<ClipperLib::cInt>::min()))
		{}
	}aabb;

	void generateLines(ClipperLib::Paths& originPloy, ClipperLib::Paths& newPaths, const double& laceGap, const double& laceRadius, bool isskeleton = false);
	void generateRounds(const ClipperLib::Paths& originPath, ClipperLib::Paths& newPloy, const double& laceGap, const double& laceRadius);

	bool isCollision(const ClipperLib::Paths& paths, const ClipperLib::IntPoint& point, const double& smallest_dist);

	inline double vSize2f(const ClipperLib::IntPoint& p0, const ClipperLib::IntPoint& p1)
	{
		return sqrt(pow((p1.X - p0.X), 2) + pow((p1.Y - p0.Y), 2)) / 1000.0;
	}

	int pathCount(ClipperLib::Path* path, const ClipperLib::IntPoint& p);
	bool deletePoint(ClipperLib::Path* path, int index);
	int findNext(ClipperLib::Path* path, ClipperLib::IntPoint point);
	void sortPath(ClipperLib::Path* path, ClipperLib::Paths* paths,bool getPerLine = false);

	float PointTOline(ClipperLib::IntPoint const& a, ClipperLib::IntPoint const& b, ClipperLib::IntPoint const& p);
	float getMinLen(const ClipperLib::Paths& paths, const ClipperLib::IntPoint point);
	float optimizePaths(ClipperLib::Paths& paths, ClipperLib::Paths& pathOrigin);

	aabb getAABB(ClipperLib::Path* path);
}

#endif // FMESH_SPECIALPOLY_1604475054469_H