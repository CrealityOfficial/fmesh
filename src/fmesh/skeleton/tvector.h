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
	TVector XMultiply(TVector RVect);      //向量叉乘
	double DotMultiply(TVector RVect);    //向量点乘
    TVector PointVector(TPoint m_Point);             //点的向量
	//bool  CheckParallel(TVector RVect);          //判断向量是否平行
	double CheckDirection(TVector RVect);         //判断向量的方向
	double  VectorLength();                     //向量的模

public:
	                  //向量的方向
	double m_VectorLength;              //向量的模


};

#endif // __TVECTORTVECTOR_H__
