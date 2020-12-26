#pragma once
#include "tinyxml/tinyxml.h"
#include "clipper/clipper.hpp"

using namespace std;
class svg
{
public:
	svg();
	~svg();

public:
	void pauseSVG(TiXmlElement* root);
	ClipperLib::Paths* outputPaths();

	void pauseRect(float width, float height, float leftTopX, float leftTopY);
	void pauseLine(float x1, float y1, float x2, float y2);
	void pauseEllipse(float Cx, float Cy, float Rx, float Ry, float precision=200.0);
	void pausePolyline(string strPoints);
	void pausePolygon(string strPoints);
	void pausePath(string strPath);
	void pausePathMm(string stringPoint, vector<ClipperLib::DoublePoint>& vctDoublePoint);
	void pausePathLl(string stringPoint, vector<ClipperLib::DoublePoint>& vctDoublePoint);
	void pausePathHh(string stringPoint, vector<ClipperLib::DoublePoint>& vctDoublePoint);
	void pausePathVv(string stringPoint, vector<ClipperLib::DoublePoint>& vctDoublePoint);
	void pausePathCc(string stringPoint, vector<ClipperLib::DoublePoint>& vctDoublePoint);
	void pausePathQq(string stringPoint, vector<ClipperLib::DoublePoint>& vctDoublePoint);
	void pausePathTt(string stringPoint, vector<ClipperLib::DoublePoint>& vctDoublePoint);
	void pausePathSs(string stringPoint, vector<ClipperLib::DoublePoint>& vctDoublePoint);
	void pausePathAa(string stringPoint, vector<ClipperLib::DoublePoint>& vctDoublePoint);


	
	void SplitString(const std::string& Src, std::vector<std::string>& Vctdest, const std::string& c);
	void SplitStringS(const std::string& Src, std::vector<std::string>& Vctdest, std::vector<std::string>& VctS);
	ClipperLib::Path DoublePoint2IntPoint(vector<ClipperLib::DoublePoint>& vctDoublePoint);
	bool isSpechars(char chars);//过滤掉path路径中的不需要的特殊字符
	void savePenultPoint(ClipperLib::DoublePoint penultPoint, ClipperLib::DoublePoint endPoint, bool isBezier=true);
	void savePenultPoint(bool isBezier = false);


private:
	float m_scaleX;//X缩放比例
	float m_scaleY;//Y缩放比例
	ClipperLib::Paths* m_paths;
	ClipperLib::DoublePoint m_currentPosition;
	bool m_isBigchars;
	pair<bool, ClipperLib::DoublePoint> m_BezierPoint;
};


