#if !defined(__TLINETLINE_H__)
#define __TLINETLINE_H__

#define OVER -999999999999

#include "tpoint.h"
#include "tvector.h"

//直线
class TLine 
{
public:
	TLine();                         
	virtual ~TLine();	                 
public:
	TLine(TPoint m_Point,TPoint n_Point);//带参数的构造函数
	

public:
	void SetLine(TPoint m_Point,TPoint n_Point);  //设置直线的参数
	int   CheckIntesect(TLine SecondLine);
	TPoint IntersectPoint(TLine SecondLine);   //两直线相交求交点
	TVector LineVector();                       //直线的方向向量
    TLine operator = (TLine FromLine);         //直线的赋值操作
	double LineLength();                      //直线的长度
	TLine  BisectorLine(double deltax=10);                    //直线的中锤线
	double LinePointDistance(TPoint m_Point);                //点到直线的矩离
	double ValidDistance(TPoint m_Point);                  //有效距离	
	TPoint ValidInterSect(TLine SecondLine);	//求有效交点

public:
	TPoint BeginPoint;                        //直线起点
	TPoint EndPoint;                          //直线终点
};

#endif // __TLINETLINE_H__
