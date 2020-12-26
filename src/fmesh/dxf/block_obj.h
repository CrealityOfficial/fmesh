#pragma once
#include <vector>//包含头文件  
#include "clipper/clipper.hpp"
#include "cdrdxf/dxf/spline.h"

using namespace std;  
//块对象类，存放DXF的块对线，包括世界坐标、缩放比例、线、圆等对象
//线对象
class DXFLine
{
public: 
	ClipperLib::IntPoint beginpoint;
	ClipperLib::IntPoint endpoint;
};
//圆对象
class DXFCircle
{
public:
	ClipperLib::IntPoint centerpoint;
	double radius;
};
//椭圆及一段圆弧对象
class DXFEllipse
{
public:
	/*! X Coordinate of center point. */
	double cx;
	/*! Y Coordinate of center point. */
	double cy;
	/*! Z Coordinate of center point. */
	double cz;

	/*! X coordinate of the endpoint of the major axis. */
	double mx;
	/*! Y coordinate of the endpoint of the major axis. */
	double my;
	/*! Z coordinate of the endpoint of the major axis. */
	double mz;

	/*! Ratio of minor axis to major axis.. */
	double ratio;
	/*! Startangle of ellipse in rad. */
	double angle1;
	/*! Endangle of ellipse in rad. */
	double angle2;
};

//多线实体对象
class DXFPolyLineEntities
{
public:
	vector<ClipperLib::IntPoint> vertex;//顶点
	bool isclose;//闭合标志位
};

//圆弧对象对象
class DXFArc
{
public:
	ClipperLib::IntPoint centerpoint;
	double radius;
	double bangle;//起点角度
	double eangle;//终点角度
};

class BlockObj
{
public:
	BlockObj()
	{
		sx = 1000.0;
		sy = 1000.0;
		sz = 1000.0;
		ipx = 0;
		ipy = 0;
		ipz = 0;
		line.clear();
	}
	~BlockObj(){}
	//块名称
	string name;
	//缩放比例
	double sx;
	double sy;
	double sz;
	//用户坐标系相在世界坐标系中的位置
	double ipx;
	double ipy;
	double ipz;
	vector<DXFLine> line;
	vector<DXFCircle> circle;
	vector<DXFEllipse> ellipse;
	vector<DXFPolyLineEntities> polylineentities;
	vector<DXFArc> arc;
	std::vector<cdrdxf::DXFSpline> splines;
	bool drawflag;
};

