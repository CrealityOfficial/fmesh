#pragma once
#include <vector>//����ͷ�ļ�  
#include "clipper/clipper.hpp"
#include "cdrdxf/dxf/spline.h"

using namespace std;  
//������࣬���DXF�Ŀ���ߣ������������ꡢ���ű������ߡ�Բ�ȶ���
//�߶���
class DXFLine
{
public: 
	ClipperLib::IntPoint beginpoint;
	ClipperLib::IntPoint endpoint;
};
//Բ����
class DXFCircle
{
public:
	ClipperLib::IntPoint centerpoint;
	double radius;
};
//��Բ��һ��Բ������
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

//����ʵ�����
class DXFPolyLineEntities
{
public:
	vector<ClipperLib::IntPoint> vertex;//����
	bool isclose;//�պϱ�־λ
};

//Բ���������
class DXFArc
{
public:
	ClipperLib::IntPoint centerpoint;
	double radius;
	double bangle;//���Ƕ�
	double eangle;//�յ�Ƕ�
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
	//������
	string name;
	//���ű���
	double sx;
	double sy;
	double sz;
	//�û�����ϵ������������ϵ�е�λ��
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

