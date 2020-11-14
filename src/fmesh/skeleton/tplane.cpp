#include "tplane.h"
#include <math.h>

TPlane::TPlane()
{

}
TPlane::~TPlane()
{

}
void TPlane::SetPlane(TPoint m_Point,TPoint n_Point,double m_Angle )
{
	TLine  m_Line;								//保存中垂线
	TPoint m_TempPoint,n_TempPoint;             //临时保存第二条直线的终点
	double m_length;
	FirstLine.BeginPoint=m_Point;               //设置第一条直线  
	FirstLine.EndPoint=n_Point;
	SecondLine.BeginPoint=(m_Point+n_Point)/2;    //设置第2条直线   
	m_Line=FirstLine.BisectorLine(20);//中垂线
	m_length=m_Line.LineLength();

	m_TempPoint=m_Line.EndPoint;

	//夹角为90时，第二条直线的终点
	if(m_Angle==90)
	{
		n_TempPoint.x=m_Line.BeginPoint.x;
		n_TempPoint.y=m_Line.BeginPoint.y;
		n_TempPoint.z=m_length;//夹角为90，时z的取值
	    
	}
	else   //夹角为0～90时，第二条直线的终点
	{
		m_Angle=m_Angle*3.1415926/180;
		n_TempPoint.x=m_TempPoint.x;
	    n_TempPoint.y=m_TempPoint.y;
	    n_TempPoint.z=m_length*tan(m_Angle); 
	}                                          
	SecondLine.EndPoint=n_TempPoint;
}

//求直线与平面的交点
TPoint TPlane::PlaneCrossPoint(TLine CrossLine)             
{
    TPoint CrossPoint;   //保存交点
		 
	TVector a_PVector=a_PVector.PointVector(SecondLine.BeginPoint);  //平面的起点
	TVector b_PVector=FirstLine.LineVector();      //平面的第一条直线的方向向量
	TVector c_PVector=SecondLine.LineVector();     //平面的第2条直线的方向向量 

	TVector d_LVector=d_LVector.PointVector(CrossLine.BeginPoint);    //直线的起点
	TVector e_LVector=CrossLine.LineVector();       //直线的方向向量

	TVector m_Temp=b_PVector.XMultiply(c_PVector);
	double s=m_Temp.DotMultiply(a_PVector);
	double r=m_Temp.DotMultiply(d_LVector);
	double t=m_Temp.DotMultiply(e_LVector);
	double m_Value=(s-r)/t;
	return CrossPoint;
      		   
}

//两不相邻平面求交
TLine TPlane::PlaneCrossLine(TPlane SecondPlane,TPoint m_Start)
{
	TLine m_CrossLine;
    TVector FPlaneVector=this->GetVerticalVetor();
	TVector SPlaneVector=SecondPlane.GetVerticalVetor();

	TVector LVector=FPlaneVector.XMultiply(SPlaneVector);
	double s=LVector.VectorLength();
    LVector.x=LVector.x/s;
	LVector.y=LVector.y/s;
    LVector.z=LVector.z/s;

	if(LVector.z<0)
	{
		LVector.x=-LVector.x;
		LVector.y=-LVector.y;
		LVector.z=-LVector.z;
	}

	m_CrossLine.BeginPoint=m_Start;//PlaneCrossPoint(SecondPlane.SecondLine);  
	m_CrossLine.EndPoint.x=m_CrossLine.BeginPoint.x+LVector.x;
	m_CrossLine.EndPoint.y=m_CrossLine.BeginPoint.y+LVector.y;
	m_CrossLine.EndPoint.z=m_CrossLine.BeginPoint.z+LVector.z;

	return m_CrossLine;
}
//相邻平面求交
TLine TPlane::NearPlaneCrossLine(TPlane SecondPlane)
{
	TLine m_CrossLine;
    
	m_CrossLine=PlaneCrossLine(SecondPlane,FirstLine.EndPoint);
	return m_CrossLine;

}
//直线与平面的位置情况：平行0，相交1。
int TPlane::CheckLinePosition(TLine m_nLine)
{
	TVector m_PVector,c_LVector,m_Temp;
	c_LVector=m_nLine.LineVector();
	m_PVector=this->GetVerticalVetor();
	double temp;
	temp=c_LVector.DotMultiply(m_PVector);
	if(temp==0)
		return 0;                   //直线与平面平行
	else
		return 1;                   //直线与平面相交                                  
}
//求平面的法向量
TVector TPlane::GetVerticalVetor()
{
	TVector a_PVector,b_PVector,n_PVector;
	a_PVector=FirstLine.LineVector();                 //平面的第一条直线的方向向量
	b_PVector=SecondLine.LineVector();//平面的第一条直线的方向向量

    n_PVector=a_PVector.XMultiply(b_PVector);
	return n_PVector;
}