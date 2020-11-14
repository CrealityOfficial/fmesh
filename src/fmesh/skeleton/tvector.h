#if !defined(__TVECTORTVECTOR_H__)
#define __TVECTORTVECTOR_H__

#include "tpoint.h"
#include "math.h"

class TVector : public TPoint  
{
public:
	TVector();                            
	virtual ~TVector();                  
	TVector (TPoint m1,double m2);        
public:
	TVector XMultiply(TVector RVect);      //�������
	double DotMultiply(TVector RVect);    //�������
    TVector PointVector(TPoint m_Point);             //�������
	//bool  CheckParallel(TVector RVect);          //�ж������Ƿ�ƽ��
	double CheckDirection(TVector RVect);         //�ж������ķ���
	double  VectorLength();                     //������ģ

public:
	                  //�����ķ���
	double m_VectorLength;              //������ģ


};

#endif // __TVECTORTVECTOR_H__
