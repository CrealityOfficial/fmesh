#ifndef FMESH_MULTISVG_1609985536284_H
#define FMESH_MULTISVG_1609985536284_H
#include "clipperxyz/clipper.hpp"
#include <limits>

#define CLIPPERPOINT(x, y) ClipperLibXYZ::IntPoint((int)(1000.0f * x), (int)(1000.0f * y))
namespace fmesh
{
	typedef struct SAABB
	{
		ClipperLibXYZ::IntPoint pMmin;
		ClipperLibXYZ::IntPoint pMax;
		SAABB():pMmin(std::numeric_limits<ClipperLibXYZ::cInt>::max(), std::numeric_limits<ClipperLibXYZ::cInt>::max())
			,pMax(std::numeric_limits<ClipperLibXYZ::cInt>::min(), std::numeric_limits<ClipperLibXYZ::cInt>::min())
		{}
	}aabb;

	class MultiSVG
	{
	public:
		MultiSVG();
		~MultiSVG();

		void addRect(float width, float height, float leftTopX, float leftTopY);
		void addEllipse(float cx, float cy, float rx, float ry, int precision = 200);
		void addPolygonS(const std::string& polygon);
		void addPathS(const std::string& path);
		void addLine(float start_x, float start_y, float end_x, float end_y);
		void addPolyLine(const std::string& polygon);

		std::vector<ClipperLibXYZ::Paths*> take();
	protected:
		void SplitString(const std::string& Src, std::vector<std::string>& Vctdest, const std::string& c);
		void SplitStringS(const std::string& Src, std::vector<std::string>& Vctdest, std::vector<std::string>& VctS);
		bool isSpechars(char chars);

		aabb getAABB(ClipperLibXYZ::Path* path);
		bool intersectAABB(const aabb& a, const aabb& b);

		void pausePathMm(std::string stringPoint);
		void pausePathLl(std::string stringPoint);
		void pausePathHh(std::string stringPoint);
		void pausePathVv(std::string stringPoint);
		void pausePathCc(std::string stringPoint);
		void pausePathQq(std::string stringPoint);
		void pausePathTt(std::string stringPoint);
		void pausePathSs(std::string stringPoint);
		void pausePathAa(std::string stringPoint);
		void savePenultPoint(ClipperLibXYZ::DoublePoint penultPoint, ClipperLibXYZ::DoublePoint endPoint, bool isBezier = true);
		void savePenultPoint(bool isBezier = false);
	protected:
		std::vector<ClipperLibXYZ::Paths*> m_pathses;
		std::vector<ClipperLibXYZ::Path*> m_path;

		ClipperLibXYZ::DoublePoint m_currentPosition;
		bool m_isBigchars;
		std::pair<bool, ClipperLibXYZ::DoublePoint> m_BezierPoint;
		ClipperLibXYZ::Path m_curPath;
	};
}

#endif // FMESH_MULTISVG_1609985536284_H