#pragma once
#include "tinyxml/tinyxml.h"
#include "clipperxyz/clipper.hpp"

#define  SVG_SVG "svg"//svg视图大小
#define  SVG_LINE "line"//线段
#define  SVG_POLYLINE "polyline"//线段
#define  SVG_RECT "rect"//矩形
#define  SVG_CIRCLE "circle"//圆
#define  SVG_ELLIPSE "ellipse"//椭圆
#define  SVG_POLYGON "polygon"//多边形
#define  SVG_PATH "path"//路径

using namespace std;
class svg
{
public:
	svg();
	~svg();

public:
	void pauseSVG(TiXmlElement* root);
	void pauseSVGEX(TiXmlElement* root);
	ClipperLibXYZ::Paths* outputPaths();

	void pauseRect(float width, float height, float leftTopX, float leftTopY);
	void pauseLine(float x1, float y1, float x2, float y2);
	void pauseEllipse(float Cx, float Cy, float Rx, float Ry, float precision=200.0);
	void pausePolyline(string strPoints);
	void pausePolygon(string strPoints);
	void pausePath(string strPath);
	void pausePathMm(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint);
	void pausePathLl(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint);
	void pausePathHh(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint);
	void pausePathVv(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint);
	void pausePathCc(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint);
	void pausePathQq(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint);
	void pausePathTt(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint);
	void pausePathSs(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint);
	void pausePathAa(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint);


	
	void SplitString(const std::string& Src, std::vector<std::string>& Vctdest, const std::string& c);
	void SplitStringS(const std::string& Src, std::vector<std::string>& Vctdest, std::vector<std::string>& VctS);
	ClipperLibXYZ::Path DoublePoint2IntPoint(vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint);
	ClipperLibXYZ::Path DoublePoint2IntPointEX(vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint);
	bool isSpechars(char chars);//过滤掉path路径中的不需要的特殊字符
	void savePenultPoint(ClipperLibXYZ::DoublePoint penultPoint, ClipperLibXYZ::DoublePoint endPoint, bool isBezier=true);
	void savePenultPoint(bool isBezier = false);


private:
	float m_scaleX;//X缩放比例
	float m_scaleY;//Y缩放比例
	float m_currentMatrix3x3[3][3];
	ClipperLibXYZ::Paths* m_paths;
	ClipperLibXYZ::DoublePoint m_currentPosition;
	bool m_isBigchars;
	pair<bool, ClipperLibXYZ::DoublePoint> m_BezierPoint;
};


