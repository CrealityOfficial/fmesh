#if !defined(__TRIDEGLINETRIDEGLINE_H__)
#define __TRIDEGLINETRIDEGLINE_H__

#include "tpolygon.h"
#include "tplane.h"
#include <clipper/clipper.hpp>
#include "fmesh/generate/patch.h"

class TRidegLine:public TPlane
{
public:
	TRidegLine();
	TRidegLine(TPolygon m_nPoly,double m_nAngle);
	virtual ~TRidegLine();

public:
	TPolygon  m_Poly;    
	int m_Counter;    
    double m_Angle;	
	std::vector<TPlane> m_RidegPlane;     //�ݶ��ĸ���
	std::vector<TLine> m_Edges;			 //�ݶ�����εı�
	std::vector<TLine> m_RidegLine;     //�ݼ��� 
	std::vector<TLine> m_CrossLine;    //������Ľ�������
	std::vector<TPoint> m_CrossPoint;    //���ڽ��ߵĽ���

	std::vector<TPoint>m_nPolygon;     //����εĶ�������
	int  n_VertexNum;           //����εĶ�����					

public:
	void SetDate(TPolygon& m_nPoly, double m_Angle);
	void SetRidegPlane();       //�����ݶ�����
    void SetPolyEdge();         //���ö���εı�
	void SetCrossLine();       //����������Ľ���
	void SetCrossPoint();      //�������ڽ��ߵĽ���

	ClipperLib::Path* excute(ClipperLib::Path* path, double z);
	void SetData(ClipperLib::Path* path);  

	void operator =(TRidegLine RLine);

	struct CInsectLine		//�潻�߽ṹ��
	{
		TLine m_Line;
		int PL_IDa;
		int PL_IDb;
	};

	std::vector<TPoint>	m_AxesPoints;	//�Ǽ��߽���
	std::vector<CInsectLine> m_InsectLines;		//�潻������
	int  FindLowest(TPoint& LowestPnt);		//������͵�
	int  FindUncommon(TPoint& LowestPnt,int& index);	//������͵��������
	void AddNewAxes(int Position,TPoint tPnt);		//������������͵���ص��潻��
	void UpdateILines(int Position,TPoint tPnt);	//�����潻������
	void CreateAxesLine();	//����������
  
};

#endif // __TRIDEGLINETRIDEGLINE_H__
