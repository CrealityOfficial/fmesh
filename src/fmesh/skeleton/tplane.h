#if !defined(__TPLANETPLANE_H__)
#define __TPLANETPLANE_H__

#include "tline.h"
//平面
class TPlane 
{
public:
	TPlane();
	virtual ~TPlane();
public:
	TLine FirstLine;
	TLine SecondLine;
	double  m_Angle;

public:
    void   SetPlane(TPoint m_Point,TPoint n_Point,double m_Angle);   //设置平面的参数
	TPoint PlaneCrossPoint(TLine CrossLine);       //求直线与平面的交线
	TLine  PlaneCrossLine(TPlane SecondPlane,TPoint m_Start);     //求两不相邻平面的交线
    TLine  NearPlaneCrossLine(TPlane SecondPlane);               //求两相邻平面的交线
	int    CheckLinePosition(TLine m_nLine);         //判断平面与直线的位置关系
	TVector GetVerticalVetor();                        //求平面的法向量
};

#endif // __TPLANETPLANE_H__
