/*
 * @file test_creationclass.cpp
 */

/*****************************************************************************
**  $Id: test_creationclass.cpp 8865 2008-02-04 18:54:02Z andrew $
**
**  This is part of the dxflib library
**  Copyright (C) 2001 Andrew Mustun
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU Library General Public License as
**  published by the Free Software Foundation.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Library General Public License for more details.
**
**  You should have received a copy of the GNU Library General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
******************************************************************************/

#include "test_creationclass.h"

#include <iostream>
#include <stdio.h>
double const pi = 3.141592653589793238;

/**
 * Default constructor.
 */
Test_CreationClass::Test_CreationClass() 
{
	BlockObj newblock;
	newblock.drawflag = true;
	newblock.name = "entities";
	myblock.push_back(newblock);//先插入一个空块，以供后面处理
}


/**
 * Sample implementation of the method which handles layers.
 */
void Test_CreationClass::addLayer(const DL_LayerData& data) {
    printf("LAYER: %s flags: %d\n", data.name.c_str(), data.flags);
    printAttributes();
}

/**
 * Sample implementation of the method which handles point entities.
 */
void Test_CreationClass::addPoint(const DL_PointData& data) {
    printf("POINT    (%6.3f, %6.3f, %6.3f)\n",
           data.x, data.y, data.z);
    printAttributes();
}

/**
 * Sample implementation of the method which handles line entities.
 */
void Test_CreationClass::addLine(const DL_LineData& data) 
{
	DXFLine myline;
	int scaleX = myblock[myblock.size() - 1].sx;
	int scaleY = myblock[myblock.size() - 1].sy;
	myline.beginpoint = ClipperLib::IntPoint(data.x1 * scaleX, data.y1 * scaleY);
	myline.endpoint = ClipperLib::IntPoint(data.x2 * scaleX, data.y2 * scaleY);
	myblock[myblock.size() - 1].line.push_back(myline);
}

/**
 * Sample implementation of the method which handles arc entities.
 */
void Test_CreationClass::addArc(const DL_ArcData& data) 
{
	DXFArc myarc;
	int scaleX = myblock[myblock.size() - 1].sx;
	int scaleY = myblock[myblock.size() - 1].sy;
	myarc.centerpoint = ClipperLib::IntPoint(data.cx* scaleX, data.cy* scaleY);
	myarc.radius = data.radius* scaleY;
	myarc.bangle = data.angle1;
	myarc.eangle = data.angle2;
	myblock[myblock.size() - 1].arc.push_back(myarc);
   /* printf("ARC      (%6.3f, %6.3f, %6.3f) %6.3f, %6.3f, %6.3f\n",
           data.cx, data.cy, data.cz,
           data.radius, data.angle1, data.angle2);
    printAttributes();*/
}

/**
 * Sample implementation of the method which handles circle entities.
 */
void Test_CreationClass::addCircle(const DL_CircleData& data) 
{
	DXFCircle mycircle;
	int scaleX = myblock[myblock.size() - 1].sx;
	int scaleY = myblock[myblock.size() - 1].sy;
	mycircle.centerpoint = ClipperLib::IntPoint(data.cx* scaleX, data.cy* scaleY);
	mycircle.radius = data.radius* scaleX;
	myblock[myblock.size() - 1].circle.push_back(mycircle);
}

/**
 * Sample implementation of the method which handles Ellipse entities.
 *cx，cy，cz为椭圆圆心的x,y,z坐标。mx，my，mz为相对于中心的长轴端点的x,y,z坐标，
 *ratio为短轴与长轴的比例，可以用来计算长短轴半径。angle1，angle2为起始和终止角度（弧度制）。
 */
void Test_CreationClass::addEllipse(const DL_EllipseData& data)
{
	int scaleX = myblock[myblock.size() - 1].sx;
	int scaleY = myblock[myblock.size() - 1].sy;

	DXFEllipse ellipse;
	ellipse.angle1 = data.angle1;
	ellipse.angle2 = data.angle2;
	ellipse.cx = data.cx* scaleX;
	ellipse.cy = data.cy* scaleY;
	ellipse.cz = data.cz;
	ellipse.mx = data.mx * scaleX;
	ellipse.my = data.my * scaleY;
	ellipse.mz = data.mz;
	ellipse.ratio = data.ratio;
	myblock[myblock.size() - 1].ellipse.push_back(ellipse);
}

/**
 * Sample implementation of the method which handles polyline entities.
 * 多线实体起点标志位
 */
void Test_CreationClass::addPolyline(const DL_PolylineData& data) 
{
	DXFPolyLineEntities mypolylineentities;
	mypolylineentities.isclose = data.flags;//闭合标志位，1表示闭合，0表示非闭合
	myblock[myblock.size() - 1].polylineentities.push_back(mypolylineentities);
}

/**
 * Sample implementation of the method which handles vertices.
 * 顶点对像，描绘多线实体
 */
void Test_CreationClass::addVertex(const DL_VertexData& data) 
{
    int scaleX = myblock[myblock.size() - 1].sx;
    int scaleY = myblock[myblock.size() - 1].sy;
    ClipperLib::IntPoint myvertex= ClipperLib::IntPoint(data.x * scaleX, data.y * scaleY);
	myblock[myblock.size() - 1].polylineentities[myblock[myblock.size() - 1].polylineentities.size() - 1].vertex.push_back(myvertex);
}

void Test_CreationClass::add3dFace(const DL_3dFaceData& data)
{
	int scaleX = myblock[myblock.size() - 1].sx;
	int scaleY = myblock[myblock.size() - 1].sy;
    int scaleZ = myblock[myblock.size() - 1].sz;
    printf("3DFACE\n");
    for (int i=0; i<4; i++) {
        printf("   corner %d: %6.3f %6.3f %6.3f\n", 
            i, data.x[i]* scaleX, data.y[i]* scaleY, data.z[i]* scaleZ);
    }
    printAttributes();
}

void Test_CreationClass::addInsert(const DL_InsertData & data)
{
	vector<BlockObj> ::iterator itor;//容器迭代器  
	itor = myblock.begin();
	while (itor != myblock.end())
	{
		if (!itor->name.compare(data.name))//插入块名称比较，相同则表示插入该块
		{
			itor->ipx = data.ipx;
			itor->ipy = data.ipy;
			itor->ipz = data.ipz;
			itor->sx = data.sx;
			itor->sy = data.sy;
			itor->sz = data.sz;
			itor->drawflag = true;//只有被插入的对象才进行绘制
			break;
		}
		itor++;
	}
}

void Test_CreationClass::addBlock(const DL_BlockData & data)
{
	myblock[myblock.size() - 1].name=data.name;
	myblock[myblock.size() - 1].drawflag = false;
}

void Test_CreationClass::endBlock()//因为dxflib没给出ENTITIES的标志位，为方便，将entitues也当成一个需要绘制的块进行处理，与正在的block的区别只在于块在插入的时候才绘制，而entities则一定会绘制 
{
	BlockObj newblock;
	newblock.drawflag = true;
	newblock.name = "entities";
	myblock.push_back(newblock);
}

void Test_CreationClass::addSpline(const DL_SplineData& data)
{
	myblock.back().splines.emplace_back(cdrdxf::DXFSpline());
	m_currentSpline = &myblock.back().splines.back();
	m_currentSpline->degree = data.degree;
	m_currentSpline->nKnots = data.nKnots;
	m_currentSpline->flags = data.flags;
	m_currentSpline->nControl = data.nControl;
}

void Test_CreationClass::addControlPoint(const DL_ControlPointData& data)
{
	if (m_currentSpline)
	{
		m_currentSpline->controlPoints.push_back(cdrdxf::Point(data.x, data.y, data.z));
	}
}

void Test_CreationClass::addFitPoint(const DL_FitPointData& data)
{

}

void Test_CreationClass::addKnot(const DL_KnotData& knot)
{
	if (m_currentSpline)
	{
		m_currentSpline->knots.push_back(knot.k);
	}
}

std::vector<cdrdxf::DXFSpline*> Test_CreationClass::splines()
{
	std::vector<cdrdxf::DXFSpline*> rt;
	for (BlockObj& block : myblock)
	{
		for (cdrdxf::DXFSpline& spline : block.splines)
			if (spline.degree >= 2 && spline.degree <= 3
				&& spline.nControl >= (spline.degree + 1))
				rt.push_back(&spline);
	}

	return rt;
}

void Test_CreationClass::printAttributes() 
{
    printf("  Attributes: Layer: %s, ", attributes.getLayer().c_str());
    printf(" Color: ");
    if (attributes.getColor()==256)	{
        printf("BYLAYER");
    } else if (attributes.getColor()==0) {
        printf("BYBLOCK");
    } else {
        printf("%d", attributes.getColor());
    }
    printf(" Width: ");
    if (attributes.getWidth()==-1) {
        printf("BYLAYER");
    } else if (attributes.getWidth()==-2) {
        printf("BYBLOCK");
    } else if (attributes.getWidth()==-3) {
        printf("DEFAULT");
    } else {
        printf("%d", attributes.getWidth());
    }
    printf(" Type: %s\n", attributes.getLinetype().c_str());
}

void Test_CreationClass::myblock2Paths(ClipperLib::Paths* paths)
{
	for (BlockObj block : myblock)
	{
		//如果line 是闭合的则添加，否则不添加
		if (block.line.size() /*&& block.line[0].beginpoint == block.line[block.line.size() - 1].endpoint*/)
		{
			ClipperLib::Path linePath;
			for (DXFLine line : block.line)
			{
				linePath.push_back(line.beginpoint);
				linePath.push_back(line.endpoint);

			}
			paths->push_back(linePath); 
		}

		for (DXFCircle circle : block.circle)
		{
			ClipperLib::Path circlePath;
			for (unsigned int i = 0; i < 200; i++)
			{
				float Angle = 2*pi * (i / 200.0);
				ClipperLib::IntPoint Point;
				Point.X = (circle.centerpoint.X + circle.radius * std::cos(Angle));
				Point.Y = (circle.centerpoint.Y - circle.radius * std::sin(Angle));
				circlePath.push_back(Point);
			}
			paths->push_back(circlePath);
		}

		for (DXFEllipse ellipse : block.ellipse)
		{
			ClipperLib::Path ellipsePath;
			double valueB = sqrt(pow(ellipse.mx, 2) + pow(ellipse.my, 2));
		    double valueA = ellipse.ratio * valueB;
			for (unsigned int i = 0; i < 200; i++)
			{
				float Angle = ellipse.angle1 + (ellipse.angle2 - ellipse.angle1) * (i / 200.00);
				ClipperLib::IntPoint Point;
				Point.X = 1000 * (ellipse.cx + valueB * std::cos(Angle));
				Point.Y = 1000 * (ellipse.cy - valueA * std::sin(Angle));
				ellipsePath.push_back(Point);
			}
			paths->push_back(ellipsePath);
		}

		for (DXFArc arc: block.arc)
		{
			ClipperLib::Path arcPath;
			for (unsigned int i = 0; i < 200; i++)
			{
				float Angle = arc.bangle + (arc.bangle - arc.eangle) * (i / 200.00);
				ClipperLib::IntPoint Point;
				Point.X = (arc.centerpoint.X + arc.radius * std::cos(Angle));
				Point.Y = (arc.centerpoint.Y - arc.radius * std::sin(Angle));
				arcPath.push_back(Point);
			}
			paths->push_back(arcPath);
		}

		for (DXFPolyLineEntities PolyLineEntities : block.polylineentities)
		{
			paths->push_back(PolyLineEntities.vertex);
		}
	}
}
