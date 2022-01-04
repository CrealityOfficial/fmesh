/**
 * @file test_creationclass.h
 */

/*****************************************************************************
**  $Id: test_creationclass.h 8865 2008-02-04 18:54:02Z andrew $
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

#ifndef TEST_CREATIONCLASS_H
#define TEST_CREATIONCLASS_H

#include "dxf/dl_creationadapter.h"
#include "fmesh/dxf/block_obj.h"
/**
 * This class takes care of the entities read from the file.
 * Usually such a class would probably store the entities.
 * this one just prints some information about them to stdout.
 *
 * @author Andrew Mustun
 */
class Test_CreationClass : public DL_CreationAdapter {
public:
    Test_CreationClass();

	vector<BlockObj> myblock;//块对象定义

    virtual void addLayer(const DL_LayerData& data);
    virtual void addPoint(const DL_PointData& data);
    virtual void addLine(const DL_LineData& data);
    virtual void addArc(const DL_ArcData& data);
    virtual void addCircle(const DL_CircleData& data);
    virtual void addEllipse(const DL_EllipseData& data);
    virtual void addPolyline(const DL_PolylineData& data);
    virtual void addVertex(const DL_VertexData& data);
    virtual void add3dFace(const DL_3dFaceData& data);
	//插入对象
	virtual void addInsert(const DL_InsertData& data);
	//插入块
	virtual void addBlock(const DL_BlockData& data);
	virtual void endBlock();
	
    void addSpline(const DL_SplineData&) override;
    void addControlPoint(const DL_ControlPointData&) override;
    void addFitPoint(const DL_FitPointData&) override;
    void addKnot(const DL_KnotData&) override;
    std::vector<cdrdxf::DXFSpline*> splines();

    void printAttributes();
	//Block 转paths
    void myblock2Paths(ClipperLibXYZ::Paths* paths);

    void myblock2MultiPaths(std::vector<ClipperLibXYZ::Paths*>& vctPaths);

private:
    void dealLine(vector<DXFLine>& lines, ClipperLibXYZ::Paths* paths);
    void dealCircle(vector<DXFCircle>& circles, ClipperLibXYZ::Paths* paths);
    void dealEllipse(vector<DXFEllipse>& ellipses, ClipperLibXYZ::Paths* paths);
    void dealPolylineentities(vector<DXFPolyLineEntities>& polylineentitiess, ClipperLibXYZ::Paths* paths);
    void dealArc(vector<DXFArc>& arcs, ClipperLibXYZ::Paths* paths);
    void dealSplines(vector<cdrdxf::DXFSpline>& spliness, ClipperLibXYZ::Paths* paths);
protected:
    cdrdxf::DXFSpline* m_currentSpline;
};

#endif
