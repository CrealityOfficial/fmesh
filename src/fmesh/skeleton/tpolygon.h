#if !defined(__TPOLYGONTPOLYGON_H__)
#define __TPOLYGONTPOLYGON_H__

#include "tpoint.h"
#include <vector>
class TPolygon  
{
public:
	TPolygon();
	TPolygon(TPolygon &temp);
	virtual ~TPolygon();

public:
     std::vector<TPoint> m_Polygon;     //����εĶ�������
     int  m_VertexNum;     //����εĶ�����
public:
	void SetData(std::vector<TPoint>& n_Polygon);  //���¶����
	TPoint GetPoint(int which);   //��ȡ����	
	void SortVertex();	 //��ʱ������
    void SortAngle();    //���������
	bool PointPosition(TPoint m_Point);	 //��Ͷ���ε�λ�ù�ϵ
	void operator =(const TPolygon& poly);
	void Clear();
	double GetArea();     //�����ε����
};

#endif // __TPOLYGONTPOLYGON_H__
