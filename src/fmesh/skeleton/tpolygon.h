#if !defined(__TPOLYGONTPOLYGON_H__)
#define __TPOLYGONTPOLYGON_H__

#include "tpoint.h"
#include <vector>
class TPolygon  
{
public:
	TPolygon();
	TPolygon(TPolygon &temp);
	virtual ~TPolygon();

public:
     std::vector<TPoint> m_Polygon;     //多边形的顶点数组
     int  m_VertexNum;     //多边形的顶点数
public:
	void SetData(std::vector<TPoint>& n_Polygon);  //更新多边形
	TPoint GetPoint(int which);   //获取顶点	
	void SortVertex();	 //逆时针排序
    void SortAngle();    //排序点坐标
	bool PointPosition(TPoint m_Point);	 //点和多边形的位置关系
	void operator =(const TPolygon& poly);
	void Clear();
	double GetArea();     //求多边形的面积
};

#endif // __TPOLYGONTPOLYGON_H__
