#ifndef FMESH_SKELETON2POLYTREE_1604475054469_H
#define FMESH_SKELETON2POLYTREE_1604475054469_H
#include "clipperxyz/clipper.hpp"

namespace fmesh
{
	typedef ClipperLibXYZ::IntPoint skelepoint;

	void dealSkeleton(ClipperLibXYZ::PolyTree& roofLine, ClipperLibXYZ::Paths* paths);
	void removeRePoint(std::vector<skelepoint>& listPointF, std::vector<skelepoint>& listPointE);
	bool deletePoint(std::vector<skelepoint>& listPoint, skelepoint point);
	bool deletePoints(std::vector<ClipperLibXYZ::IntPoint>& listPointF, std::vector<ClipperLibXYZ::IntPoint>& listPointE, ClipperLibXYZ::IntPoint pointF, ClipperLibXYZ::IntPoint pointE);
	void findOptimalPoints(std::vector<skelepoint>& listPointF, std::vector<skelepoint>& optimalPoint, skelepoint point);
	int findOptimalPos(std::vector<skelepoint>& listPointF, std::vector<skelepoint>& listPointE);
	int findPos(std::vector<skelepoint>& listPointF, skelepoint point);
	int findPoint(std::vector<ClipperLibXYZ::IntPoint>& listPointF, std::vector<ClipperLibXYZ::IntPoint>& optimalPoint);
	int findCount(std::vector<skelepoint>& listPointF, skelepoint point);
	int reversePoint(std::vector<skelepoint>& listPointF, std::vector<skelepoint>& listPointE, skelepoint pointF, skelepoint pointE);
	double averageLen(ClipperLibXYZ::PolyTree& source, ClipperLibXYZ::Paths& paths);
	bool isEnds(ClipperLibXYZ::PolyTree& source, std::vector<ClipperLibXYZ::IntPoint>& path, std::vector<ClipperLibXYZ::IntPoint>::iterator iter, double alen, bool& allDel);
	bool needDelPath(ClipperLibXYZ::PolyTree& source, ClipperLibXYZ::Path& path, double alen);
	bool isIntersect(ClipperLibXYZ::Paths& paths, ClipperLibXYZ::IntPoint point);
	double getMinLen(ClipperLibXYZ::PolyTree& source, ClipperLibXYZ::IntPoint point);
	double PointTolines(ClipperLibXYZ::IntPoint const& a, ClipperLibXYZ::IntPoint const& b, ClipperLibXYZ::IntPoint const& p);
}

#endif // FMESH_SKELETON2POLYTREE_1604475054469_H