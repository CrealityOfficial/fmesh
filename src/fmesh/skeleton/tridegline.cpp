#include "tridegline.h"
#include "fmesh/generate/triangularization.h"

TRidegLine::TRidegLine()
{
	m_Counter=0;
}
TRidegLine::TRidegLine(TPolygon m_nPoly,double m_nAngle)
{
    m_Counter=m_nPoly.m_VertexNum;
	m_Poly=m_nPoly;
	m_Angle=m_nAngle;

	SetRidegPlane();
	SetPolyEdge();
	SetCrossLine();
	SetCrossPoint();
}


TRidegLine::~TRidegLine()
{
	m_CrossLine.clear();
	m_CrossPoint.clear();
	m_RidegLine.clear();
	m_RidegPlane.clear();
	m_Edges.clear();
}
//�����ݶ�����
void TRidegLine::SetRidegPlane()
{
	
    TPlane TempPlane;
	for(int i=0;i<m_Counter;i++)
	{
		TempPlane.SetPlane(m_Poly.m_Polygon[i],
			m_Poly.m_Polygon[(i+1)%m_Counter],m_Angle);
		
		m_RidegPlane.push_back(TempPlane);
	}
	
}
//��ʼ�����ݳ�Ա
void TRidegLine::SetDate(TPolygon& m_nPoly,double m_nAngle)
{	
	m_Counter=m_nPoly.m_VertexNum;
	m_Poly=m_nPoly;
	m_Angle=m_nAngle;

	SetRidegPlane();//��ȡ��+�д�����ɵ���	
	SetPolyEdge();	//����α���
	SetCrossLine(); //�����߶ν���
	SetCrossPoint();  //��������߶ν��߽���
}

void TRidegLine::SetData(ClipperLib::Path* path)
{
	int size = (int)path->size();
	for (int i = 0; i < size; ++i)
	{
		TPoint tmp;
		tmp.x = (double)path->at(i).X/1000.0;
		tmp.y = (double)path->at(i).Y / 1000.0;
		tmp.z = (double)path->at(i).Z / 1000.0;
		m_nPolygon.push_back(tmp);
	}
}

//���ö���εı�
void TRidegLine::SetPolyEdge()
{
	TLine TempLine;
	for(int i=0;i<m_Counter;i++)
	{
		TempLine.SetLine(m_Poly.m_Polygon[i],
							m_Poly.m_Polygon[(i+1)%m_Counter]);
		m_Edges.push_back(TempLine);
	} 
}

//����������Ľ���
void TRidegLine::SetCrossLine()
{
	TLine TempLine;
	 for(int i=0;i<m_Counter;i++)
	 {
		TempLine=m_RidegPlane[(i-1+m_Counter)%m_Counter].NearPlaneCrossLine(m_RidegPlane[i]);//��ǰһ���潻��
		m_CrossLine.push_back(TempLine);
	 }
} 

//�������ڽ��ߵĽ���
void TRidegLine::SetCrossPoint()
{
	TPoint TempPoint;
	  for(int i=0;i<m_Counter;i++)
	  {
		  TempPoint=m_CrossLine[i].ValidInterSect(m_CrossLine[(i+1)%m_Counter]); 
		  m_CrossPoint.push_back(TempPoint);
	  }
}

ClipperLib::Path*  TRidegLine::excute(ClipperLib::Path* path,double z)
{
	//init
	SetData(path);

	//����
	TPolygon  n_Polygon;
	n_Polygon.SetData(m_nPolygon);  //���ݶ���ε�ֵ
	n_Polygon.SortVertex();	//������ʱ������

	SetDate(n_Polygon, 30);	//��ʼ���ݶ�����		
	CreateAxesLine();

	int num=m_AxesPoints.size();
	ClipperLib::Path* triangle = new ClipperLib::Path();
	triangle->reserve(num * 3);
	
	for (size_t i=0;i<num;i++)
	{
		ClipperLib::IntPoint  vec3;
		vec3.X = m_AxesPoints[i].x * 1000;
		vec3.Y = m_AxesPoints[i].y * 1000;
		vec3.Z = z*1000;
		triangle->push_back(vec3);
	}
	return triangle;
}

void TRidegLine::operator =(TRidegLine RLine)
{
	int Num=RLine.m_RidegLine.size();
	for(int i=0; i<Num; i++)
		m_RidegLine.push_back(RLine.m_RidegLine[i]);
}

//������͵�
int TRidegLine::FindLowest(TPoint& LowestPnt)
{
	int which=0;	//��͵�����ֵ
	int Num=m_InsectLines.size();	//�潻�߸���	
	TPoint TempPnt;	//�潻�ߵĽ���

	for(int i=0; i<Num; i++)
	{
		//�������潻�߽���,������߶���������Ľ������󽻵�
		if( (m_InsectLines[i].PL_IDa+2)%m_Counter == m_InsectLines[(i+1)%Num].PL_IDb )	
			TempPnt=m_CrossPoint[m_InsectLines[i].PL_IDb];
		else
			TempPnt=m_InsectLines[i].m_Line.ValidInterSect(
						   m_InsectLines[(i+1)%Num].m_Line);
		
		//�Ƚ�Z�����С
		if(TempPnt.x==OVER )	//���������Ч(�����ڻ������߷�������)
			continue;

		if(0==i || TempPnt.z<LowestPnt.z)
		{
			LowestPnt=TempPnt;
			which=i;
			which=FindUncommon(LowestPnt,i);	//�����������
		}			
	}
	return which;
}

//������͵��������
int TRidegLine::FindUncommon(TPoint& LowestPnt,int& index)
{
	int Num=m_InsectLines.size();
	int pos=index;	//��¼����ֵ
	//����潻�߷�����
	if(m_InsectLines[index].m_Line.BeginPoint.z >
		m_InsectLines[index].m_Line.EndPoint.z )
	{
	
		TPoint LCutPoint;	//�߱��������и�ĵ�
		LCutPoint=m_InsectLines[index].m_Line.ValidInterSect(
					 m_InsectLines[(index-1+Num)%Num].m_Line);
		
		//ȡ�Ͻ�(��)�ĵ�Ϊ��Ч��
		if(LCutPoint.z != OVER && (LCutPoint.z > LowestPnt.z))
		{
			LowestPnt=LCutPoint;
			pos=index-1;
		}
	}
	return pos;
}

//����������
void TRidegLine::CreateAxesLine()
{
	//���ɳ�ʼ���潻������
	CInsectLine ILine;	//�����潻��
	for(int i=0; i<m_Counter; i++)
	{	
		ILine.m_Line=m_CrossLine[i];
		ILine.PL_IDa=(i-1+m_Counter)%m_Counter;
		ILine.PL_IDb=i;
		m_InsectLines.push_back(ILine);
	}

	m_RidegLine.clear();

	//ѭ������������
	TPoint tPnt;	//��ͽ���
	int Position;	//��͵��������е����

	for(int i=0; i<m_Counter-2; i++)
	{
		Position=FindLowest(tPnt);	//������͵�

		if (m_Poly.PointPosition(tPnt))
		{
			AddNewAxes(Position, tPnt);	//������������͵���ص��潻��
		}

		UpdateILines(Position,tPnt);	//�����潻������	
	}
	//���һ��
	m_RidegLine.push_back(TLine(m_InsectLines[0].m_Line.BeginPoint,m_InsectLines[1].m_Line.BeginPoint));
	
}

//������������͵���ص��潻��
void TRidegLine::AddNewAxes(int Position,TPoint tPnt)
{
	TLine LineA=m_InsectLines[Position].m_Line;
	TLine LineB=m_InsectLines[(Position+1)%m_InsectLines.size()].m_Line;
	LineA.EndPoint=tPnt;	//������ֱ���յ�
	LineB.EndPoint=tPnt;
	m_AxesPoints.push_back(tPnt);	//����Ǽܽ��
	m_RidegLine.push_back(LineA);	//�����߲�������������
	m_RidegLine.push_back(LineB);
}

//�����潻������
void TRidegLine::UpdateILines(int Position,TPoint tPnt)
{
	int a=m_InsectLines[Position].PL_IDa;
	int b=m_InsectLines[(Position+1)%m_InsectLines.size()].PL_IDb;

	CInsectLine ILine;	//�����µ��潻��
	ILine.m_Line=m_RidegPlane[a].PlaneCrossLine(m_RidegPlane[b],tPnt);
	ILine.PL_IDa=a;
	ILine.PL_IDb=b;

	m_InsectLines.insert(m_InsectLines.begin()+ Position, ILine);	//�����µ��潻��
	int index;	//���潻�ߵ�λ��
	for(int i=0; i<2; i++)	//ɾ�������ɵ��潻��
	{
		index=(Position+1)%m_InsectLines.size();
		m_InsectLines.erase(m_InsectLines.begin() + index);
	}
}