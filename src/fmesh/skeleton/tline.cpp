#include "tline.h"

TLine::TLine()
{

}
TLine::~TLine()
{

}
TLine::TLine(TPoint m_Point,TPoint n_Point)
{
	BeginPoint=m_Point;
	EndPoint=n_Point;
}

//设置直线的参数
void TLine::SetLine(TPoint m_Point,TPoint n_Point)
{
	BeginPoint=m_Point;
	EndPoint=n_Point;
}

//获取直线的向量
TVector TLine::LineVector()
{
	TVector m_LineVector;
	double s=1;//LineLength()/2;
	m_LineVector.x=(EndPoint.x-BeginPoint.x)/s;
	m_LineVector.y=(EndPoint.y-BeginPoint.y)/s;
	m_LineVector.z=(EndPoint.z-BeginPoint.z)/s;
	
	return m_LineVector;
}

//判断直线是否有交点
int TLine::CheckIntesect(TLine SecondLine)
{
	TVector m_temp,n_temp,s_temp,t_temp;
	double s,m;
	TLine 	m_TempLine(BeginPoint,SecondLine.BeginPoint);

	m_temp=SecondLine.LineVector();
	n_temp=this->LineVector();
	s_temp=m_TempLine.LineVector();
    n_temp=n_temp.XMultiply(m_temp);
	s=s_temp.DotMultiply(t_temp);
	if(s!=0)
		return 2;	//异面
	else
	{
		m=n_temp.CheckDirection(m_temp);
		if(m==0)
			return 1;	//平行或重合
		else
		   return 0;
	}	
}
//直线两点之间的距离
double TLine::LineLength()
{
	double m_length;
	return m_length=EndPoint.GetPointDistance(BeginPoint);
}

//求直线的中垂线,对于xoy平面
TLine TLine::BisectorLine(double deltax)
{
	TLine  BisectorLine;
	TVector m_LineVector;
	BisectorLine.BeginPoint=(BeginPoint+EndPoint)/2;
	m_LineVector=this->LineVector();
	
	//垂线 = b(y,-x)
	BisectorLine.EndPoint.x=BisectorLine.BeginPoint.x+m_LineVector.y;
	BisectorLine.EndPoint.y=BisectorLine.BeginPoint.y-m_LineVector.x;
	BisectorLine.EndPoint.z=BisectorLine.BeginPoint.z+m_LineVector.z;
    
	return BisectorLine;

}
TLine TLine::operator =(TLine FromLine)
{
	BeginPoint=FromLine.BeginPoint;
	EndPoint=FromLine.EndPoint;
	return *this;
}

//点到直线的距离
double TLine::LinePointDistance(TPoint m_Point) 
{
	double distance;	//点到直线的距离

   if(m_Point.x==OVER && m_Point.y==OVER
	&&m_Point.z==OVER)
	   return distance=-1;
   else
   {
	   TLine PLine(this->BeginPoint,m_Point);		//将点与直线起点连接
		TVector LineVector=this->LineVector();
		TVector PLineVector=PLine.LineVector();
		TVector m_Temp;
		double LineLength=this->LineLength();
		m_Temp=PLineVector.XMultiply(LineVector);
		distance=m_Temp.VectorLength()/LineLength;
		return distance;
   }
}
double  TLine::ValidDistance(TPoint m_Point) 
{	double distance;
	TLine temp(BeginPoint,m_Point);
	TVector m_Vector=LineVector();
	TVector n_Vector=temp.LineVector();
	double s=m_Vector.CheckDirection(n_Vector);
	if(s<0)
		{	distance=LinePointDistance(m_Point);
		return distance;}
	else 

		return distance=-1;		
}

//两直线相交求交点
TPoint TLine::IntersectPoint(TLine SecondLine)
{
	TPoint IntersectPoint;
	double t=this->CheckIntesect(SecondLine);
	if(t==1||t==2)		//异面或重合
	{
		IntersectPoint.x=OVER;
		IntersectPoint.y=OVER;
		IntersectPoint.z=OVER;

    	return  IntersectPoint;
	}
	else
	{		
		TPoint m_TPoint=BeginPoint-SecondLine.BeginPoint;
		TVector m_nTVector,p_nTVector;
		m_nTVector=LineVector();
		p_nTVector=SecondLine.LineVector();
		double s,t,l;
		t=p_nTVector.y*m_TPoint.x-p_nTVector.x*m_TPoint.y;
		s=p_nTVector.x*m_nTVector.y-p_nTVector.y*m_nTVector.x;
		l=t/s;	
		IntersectPoint.x=BeginPoint.x+m_nTVector.x*l;
		IntersectPoint.y=BeginPoint.y+m_nTVector.y*l;
		IntersectPoint.z=BeginPoint.z+m_nTVector.z*l;
		return  IntersectPoint;
	}	
}

//求有效交点
TPoint TLine::ValidInterSect(TLine SecondLine)
{	
	TPoint ValidPoint;          //返回有效交点
	TVector m_Vector=LineVector();    //直线的向量
	ValidPoint=IntersectPoint(SecondLine);           //求交

	if(ValidPoint.x==OVER)	//没有交点或有无数交点
		return ValidPoint;
	else
	{
		TLine m_Line(BeginPoint,ValidPoint);
		TVector n_Vector=m_Line.LineVector();	//起点-交点向量
		double s=m_Vector.z*n_Vector.z;
		if(s>=0)	//如果方向相同则为有效交点
			return ValidPoint;
		else		//否则置为无效
		{
			ValidPoint.x=OVER;
			ValidPoint.y=OVER;
			ValidPoint.z=OVER;
			return ValidPoint;
		}
	}
}
