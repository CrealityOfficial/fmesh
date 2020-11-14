#include "tvector.h"
TVector::TVector()
{

   m_VectorLength=0;
}
TVector::~TVector()
{

}

//向量点乘函数
double TVector::DotMultiply(TVector RVect)
{
	double DMResult;    //点乘结果
	DMResult=x*RVect.x+y*RVect.y+z*RVect.z;
	return DMResult;
}
//向量叉乘
TVector TVector::XMultiply(TVector RVect)
{
	TVector XMResult;   //叉乘结果
	XMResult.x=y*RVect.z-z*RVect.y;
	XMResult.y=z*RVect.x-x*RVect.z;
	XMResult.z=x*RVect.y-y*RVect.x;
	return XMResult;
}

//判断向量的方向
double TVector::CheckDirection(TVector RVect)
{
		double temp;
	   temp=y*RVect.z-z*RVect.y+z*RVect.x-x*RVect.z
		+x*RVect.y-y*RVect.x;
	   return temp;

}
//求向量的模
double TVector::VectorLength()
{
    m_VectorLength=sqrt(x*x+y*y+z*z);       	
	return  m_VectorLength;
}
TVector TVector::PointVector(TPoint m_Point)
{
	x=m_Point.x;
	y=m_Point.y;
	z=m_Point.z;
	return *this;

}
