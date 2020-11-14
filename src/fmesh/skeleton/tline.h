#if !defined(__TLINETLINE_H__)
#define __TLINETLINE_H__

#define OVER -999999999999

#include "tpoint.h"
#include "tvector.h"

//ֱ��
class TLine 
{
public:
	TLine();                         
	virtual ~TLine();	                 
public:
	TLine(TPoint m_Point,TPoint n_Point);//�������Ĺ��캯��
	

public:
	void SetLine(TPoint m_Point,TPoint n_Point);  //����ֱ�ߵĲ���
	int   CheckIntesect(TLine SecondLine);
	TPoint IntersectPoint(TLine SecondLine);   //��ֱ���ཻ�󽻵�
	TVector LineVector();                       //ֱ�ߵķ�������
    TLine operator = (TLine FromLine);         //ֱ�ߵĸ�ֵ����
	double LineLength();                      //ֱ�ߵĳ���
	TLine  BisectorLine(double deltax=10);                    //ֱ�ߵ��д���
	double LinePointDistance(TPoint m_Point);                //�㵽ֱ�ߵľ���
	double ValidDistance(TPoint m_Point);                  //��Ч����	
	TPoint ValidInterSect(TLine SecondLine);	//����Ч����

public:
	TPoint BeginPoint;                        //ֱ�����
	TPoint EndPoint;                          //ֱ���յ�
};

#endif // __TLINETLINE_H__
