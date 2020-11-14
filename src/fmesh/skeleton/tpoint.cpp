#include "tpoint.h"
#include  "math.h"
TPoint::~TPoint()
{

}
TPoint::TPoint(double x1,double y1,double z1)
 {
	 x=x1;
	 y=y1;
	 z=z1;
 }
void TPoint::operator =(TPoint SecondPoint)
{
	x=SecondPoint.x;
    y=SecondPoint.y;
	z=SecondPoint.z;

}
TPoint TPoint::operator -(TPoint SecondPoint)
{
	return TPoint(x-SecondPoint.x, y-SecondPoint.y,z-SecondPoint.z);
}
TPoint TPoint::operator +(TPoint SecondPoint)
{
	return TPoint(x+SecondPoint.x, y+SecondPoint.y,z+SecondPoint.z);
}
bool TPoint::operator ==(TPoint SecondPoint)
{
	if(x==SecondPoint.x && y==SecondPoint.y && z==SecondPoint.z)
		return 1;
	else 
		return 0;
}
TPoint TPoint::operator *(double m_Value)
{
	return TPoint(x*m_Value, y*m_Value,z*m_Value);
}
TPoint TPoint::operator /(double m_Value)
{
	return TPoint(x/m_Value, y/m_Value,z/m_Value);
}
//两点之间距离
double TPoint::GetPointDistance(TPoint SecondPoint)
{
	double m_length;     //两点距离
    m_length=(SecondPoint.x-x)*(SecondPoint.x-x)+(SecondPoint.y-y)
		*(SecondPoint.y-y)+(SecondPoint.z-z)*(SecondPoint.z-z);
	m_length=sqrt(m_length);
	return m_length;
}
//两点连线与x轴的夹角
double TPoint::GetCos(TPoint p1)
{
	double cos,temp;
	temp=(p1.x-x)*(p1.x-x)+(p1.y-y)*(p1.y-y);
	cos=(p1.x-x)/sqrt(temp);
	 	
    return cos;
}