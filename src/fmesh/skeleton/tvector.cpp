#include "tvector.h"
TVector::TVector()
{

   m_VectorLength=0;
}
TVector::~TVector()
{

}

//������˺���
double TVector::DotMultiply(TVector RVect)
{
	double DMResult;    //��˽��
	DMResult=x*RVect.x+y*RVect.y+z*RVect.z;
	return DMResult;
}
//�������
TVector TVector::XMultiply(TVector RVect)
{
	TVector XMResult;   //��˽��
	XMResult.x=y*RVect.z-z*RVect.y;
	XMResult.y=z*RVect.x-x*RVect.z;
	XMResult.z=x*RVect.y-y*RVect.x;
	return XMResult;
}

//�ж������ķ���
double TVector::CheckDirection(TVector RVect)
{
		double temp;
	   temp=y*RVect.z-z*RVect.y+z*RVect.x-x*RVect.z
		+x*RVect.y-y*RVect.x;
	   return temp;

}
//��������ģ
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
