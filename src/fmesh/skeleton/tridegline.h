#if !defined(__TRIDEGLINETRIDEGLINE_H__)
#define __TRIDEGLINETRIDEGLINE_H__

#include "tpolygon.h"
#include "tplane.h"
#include <clipper/clipper.hpp>
#include "fmesh/generate/patch.h"

class TRidegLine:public TPlane
{
public:
	TRidegLine();
	TRidegLine(TPolygon m_nPoly,double m_nAngle);
	virtual ~TRidegLine();

public:
	TPolygon  m_Poly;    
	int m_Counter;    
    double m_Angle;	
	std::vector<TPlane> m_RidegPlane;     //屋顶的各面
	std::vector<TLine> m_Edges;			 //屋顶多边形的边
	std::vector<TLine> m_RidegLine;     //屋脊线 
	std::vector<TLine> m_CrossLine;    //相邻面的交线数组
	std::vector<TPoint> m_CrossPoint;    //相邻交线的交点

	std::vector<TPoint>m_nPolygon;     //多边形的顶点数组
	int  n_VertexNum;           //多边形的顶点数					

public:
	void SetDate(TPolygon& m_nPoly, double m_Angle);
	void SetRidegPlane();       //设置屋顶的面
    void SetPolyEdge();         //设置多边形的边
	void SetCrossLine();       //设置相邻面的交线
	void SetCrossPoint();      //设置相邻交线的交点

	ClipperLib::Path* excute(ClipperLib::Path* path, double z);
	void SetData(ClipperLib::Path* path);  

	void operator =(TRidegLine RLine);

	struct CInsectLine		//面交线结构体
	{
		TLine m_Line;
		int PL_IDa;
		int PL_IDb;
	};

	std::vector<TPoint>	m_AxesPoints;	//骨架线交点
	std::vector<CInsectLine> m_InsectLines;		//面交线链表
	int  FindLowest(TPoint& LowestPnt);		//查找最低点
	int  FindUncommon(TPoint& LowestPnt,int& index);	//查找最低点特殊情况
	void AddNewAxes(int Position,TPoint tPnt);		//插入两条与最低点相关的面交线
	void UpdateILines(int Position,TPoint tPnt);	//更新面交线链表
	void CreateAxesLine();	//构建中轴线
  
};

#endif // __TRIDEGLINETRIDEGLINE_H__
