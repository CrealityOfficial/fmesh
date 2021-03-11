#ifndef FMESH_MULTISVG_1609985536284_H
#define FMESH_MULTISVG_1609985536284_H
#include <clipper/clipper.hpp>

#define CLIPPERPOINT(x, y) ClipperLib::IntPoint((int)(1000.0f * x), (int)(1000.0f * y))
namespace fmesh
{
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

		std::vector<ClipperLib::Paths*> take();
	protected:
		void SplitString(const std::string& Src, std::vector<std::string>& Vctdest, const std::string& c);
		void SplitStringS(const std::string& Src, std::vector<std::string>& Vctdest, std::vector<std::string>& VctS);
		bool isSpechars(char chars);

		void pausePathMm(std::string stringPoint);
		void pausePathLl(std::string stringPoint);
		void pausePathHh(std::string stringPoint);
		void pausePathVv(std::string stringPoint);
		void pausePathCc(std::string stringPoint);
		void pausePathQq(std::string stringPoint);
		void pausePathTt(std::string stringPoint);
		void pausePathSs(std::string stringPoint);
		void pausePathAa(std::string stringPoint);
		void savePenultPoint(ClipperLib::DoublePoint penultPoint, ClipperLib::DoublePoint endPoint, bool isBezier = true);
		void savePenultPoint(bool isBezier = false);
	protected:
		std::vector<ClipperLib::Paths*> m_pathses;

		ClipperLib::DoublePoint m_currentPosition;
		bool m_isBigchars;
		std::pair<bool, ClipperLib::DoublePoint> m_BezierPoint;
		ClipperLib::Path m_curPath;
	};
}

#endif // FMESH_MULTISVG_1609985536284_H