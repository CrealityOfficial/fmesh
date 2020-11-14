#if !defined(__TPLANETPLANE_H__)
#define __TPLANETPLANE_H__

#include "tline.h"
//ƽ��
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
    void   SetPlane(TPoint m_Point,TPoint n_Point,double m_Angle);   //����ƽ��Ĳ���
	TPoint PlaneCrossPoint(TLine CrossLine);       //��ֱ����ƽ��Ľ���
	TLine  PlaneCrossLine(TPlane SecondPlane,TPoint m_Start);     //����������ƽ��Ľ���
    TLine  NearPlaneCrossLine(TPlane SecondPlane);               //��������ƽ��Ľ���
	int    CheckLinePosition(TLine m_nLine);         //�ж�ƽ����ֱ�ߵ�λ�ù�ϵ
	TVector GetVerticalVetor();                        //��ƽ��ķ�����
};

#endif // __TPLANETPLANE_H__
