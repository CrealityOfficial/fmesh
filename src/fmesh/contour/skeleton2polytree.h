#ifndef FMESH_SKELETON2POLYTREE_1604475054469_H
#define FMESH_SKELETON2POLYTREE_1604475054469_H
#include <clipper/clipper.hpp>

namespace fmesh
{
	typedef ClipperLib::IntPoint skelepoint;

	void dealSkeleton(ClipperLib::PolyTree& roofLine, ClipperLib::Paths* paths);
	void removeRePoint(std::vector<skelepoint>& listPointF, std::vector<skelepoint>& listPointE);
	bool deletePoint(std::vector<skelepoint>& listPoint, skelepoint point);
	bool deletePoints(std::vector<ClipperLib::IntPoint>& listPointF, std::vector<ClipperLib::IntPoint>& listPointE, ClipperLib::IntPoint pointF, ClipperLib::IntPoint pointE);
	void findOptimalPoints(std::vector<skelepoint>& listPointF, std::vector<skelepoint>& optimalPoint, skelepoint point);
	int findOptimalPos(std::vector<skelepoint>& listPointF, std::vector<skelepoint>& listPointE);
	int findPos(std::vector<skelepoint>& listPointF, skelepoint point);
	int findPoint(std::vector<ClipperLib::IntPoint>& listPointF, std::vector<ClipperLib::IntPoint>& optimalPoint);
	int findCount(std::vector<skelepoint>& listPointF, skelepoint point);
	int reversePoint(std::vector<skelepoint>& listPointF, std::vector<skelepoint>& listPointE, skelepoint pointF, skelepoint pointE);
	double averageLen(ClipperLib::PolyTree& source, ClipperLib::Paths& paths);
	bool isEnds(ClipperLib::PolyTree& source, std::vector<ClipperLib::IntPoint>& path, std::vector<ClipperLib::IntPoint>::iterator iter, double alen, bool& allDel);
	bool needDelPath(ClipperLib::PolyTree& source, ClipperLib::Path& path, double alen);
	bool isIntersect(ClipperLib::Paths& paths, ClipperLib::IntPoint point);
	double getMinLen(ClipperLib::PolyTree& source, ClipperLib::IntPoint point);
	double PointTOline(ClipperLib::IntPoint const& a, ClipperLib::IntPoint const& b, ClipperLib::IntPoint const& p);
}

#endif // FMESH_SKELETON2POLYTREE_1604475054469_H