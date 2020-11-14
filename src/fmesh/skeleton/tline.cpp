#include "tline.h"

TLine::TLine()
{

}
TLine::~TLine()
{

}
TLine::TLine(TPoint m_Point,TPoint n_Point)
{
	BeginPoint=m_Point;
	EndPoint=n_Point;
}

//����ֱ�ߵĲ���
void TLine::SetLine(TPoint m_Point,TPoint n_Point)
{
	BeginPoint=m_Point;
	EndPoint=n_Point;
}

//��ȡֱ�ߵ�����
TVector TLine::LineVector()
{
	TVector m_LineVector;
	double s=1;//LineLength()/2;
	m_LineVector.x=(EndPoint.x-BeginPoint.x)/s;
	m_LineVector.y=(EndPoint.y-BeginPoint.y)/s;
	m_LineVector.z=(EndPoint.z-BeginPoint.z)/s;
	
	return m_LineVector;
}

//�ж�ֱ���Ƿ��н���
int TLine::CheckIntesect(TLine SecondLine)
{
	TVector m_temp,n_temp,s_temp,t_temp;
	double s,m;
	TLine 	m_TempLine(BeginPoint,SecondLine.BeginPoint);

	m_temp=SecondLine.LineVector();
	n_temp=this->LineVector();
	s_temp=m_TempLine.LineVector();
    n_temp=n_temp.XMultiply(m_temp);
	s=s_temp.DotMultiply(t_temp);
	if(s!=0)
		return 2;	//����
	else
	{
		m=n_temp.CheckDirection(m_temp);
		if(m==0)
			return 1;	//ƽ�л��غ�
		else
		   return 0;
	}	
}
//ֱ������֮��ľ���
double TLine::LineLength()
{
	double m_length;
	return m_length=EndPoint.GetPointDistance(BeginPoint);
}

//��ֱ�ߵ��д���,����xoyƽ��
TLine TLine::BisectorLine(double deltax)
{
	TLine  BisectorLine;
	TVector m_LineVector;
	BisectorLine.BeginPoint=(BeginPoint+EndPoint)/2;
	m_LineVector=this->LineVector();
	
	//���� = b(y,-x)
	BisectorLine.EndPoint.x=BisectorLine.BeginPoint.x+m_LineVector.y;
	BisectorLine.EndPoint.y=BisectorLine.BeginPoint.y-m_LineVector.x;
	BisectorLine.EndPoint.z=BisectorLine.BeginPoint.z+m_LineVector.z;
    
	return BisectorLine;

}
TLine TLine::operator =(TLine FromLine)
{
	BeginPoint=FromLine.BeginPoint;
	EndPoint=FromLine.EndPoint;
	return *this;
}

//�㵽ֱ�ߵľ���
double TLine::LinePointDistance(TPoint m_Point) 
{
	double distance;	//�㵽ֱ�ߵľ���

   if(m_Point.x==OVER && m_Point.y==OVER
	&&m_Point.z==OVER)
	   return distance=-1;
   else
   {
	   TLine PLine(this->BeginPoint,m_Point);		//������ֱ���������
		TVector LineVector=this->LineVector();
		TVector PLineVector=PLine.LineVector();
		TVector m_Temp;
		double LineLength=this->LineLength();
		m_Temp=PLineVector.XMultiply(LineVector);
		distance=m_Temp.VectorLength()/LineLength;
		return distance;
   }
}
double  TLine::ValidDistance(TPoint m_Point) 
{	double distance;
	TLine temp(BeginPoint,m_Point);
	TVector m_Vector=LineVector();
	TVector n_Vector=temp.LineVector();
	double s=m_Vector.CheckDirection(n_Vector);
	if(s<0)
		{	distance=LinePointDistance(m_Point);
		return distance;}
	else 

		return distance=-1;		
}

//��ֱ���ཻ�󽻵�
TPoint TLine::IntersectPoint(TLine SecondLine)
{
	TPoint IntersectPoint;
	double t=this->CheckIntesect(SecondLine);
	if(t==1||t==2)		//������غ�
	{
		IntersectPoint.x=OVER;
		IntersectPoint.y=OVER;
		IntersectPoint.z=OVER;

    	return  IntersectPoint;
	}
	else
	{		
		TPoint m_TPoint=BeginPoint-SecondLine.BeginPoint;
		TVector m_nTVector,p_nTVector;
		m_nTVector=LineVector();
		p_nTVector=SecondLine.LineVector();
		double s,t,l;
		t=p_nTVector.y*m_TPoint.x-p_nTVector.x*m_TPoint.y;
		s=p_nTVector.x*m_nTVector.y-p_nTVector.y*m_nTVector.x;
		l=t/s;	
		IntersectPoint.x=BeginPoint.x+m_nTVector.x*l;
		IntersectPoint.y=BeginPoint.y+m_nTVector.y*l;
		IntersectPoint.z=BeginPoint.z+m_nTVector.z*l;
		return  IntersectPoint;
	}	
}

//����Ч����
TPoint TLine::ValidInterSect(TLine SecondLine)
{	
	TPoint ValidPoint;          //������Ч����
	TVector m_Vector=LineVector();    //ֱ�ߵ�����
	ValidPoint=IntersectPoint(SecondLine);           //��

	if(ValidPoint.x==OVER)	//û�н��������������
		return ValidPoint;
	else
	{
		TLine m_Line(BeginPoint,ValidPoint);
		TVector n_Vector=m_Line.LineVector();	//���-��������
		double s=m_Vector.z*n_Vector.z;
		if(s>=0)	//���������ͬ��Ϊ��Ч����
			return ValidPoint;
		else		//������Ϊ��Ч
		{
			ValidPoint.x=OVER;
			ValidPoint.y=OVER;
			ValidPoint.z=OVER;
			return ValidPoint;
		}
	}
}
