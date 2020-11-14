#include "tplane.h"
#include <math.h>

TPlane::TPlane()
{

}
TPlane::~TPlane()
{

}
void TPlane::SetPlane(TPoint m_Point,TPoint n_Point,double m_Angle )
{
	TLine  m_Line;								//�����д���
	TPoint m_TempPoint,n_TempPoint;             //��ʱ����ڶ���ֱ�ߵ��յ�
	double m_length;
	FirstLine.BeginPoint=m_Point;               //���õ�һ��ֱ��  
	FirstLine.EndPoint=n_Point;
	SecondLine.BeginPoint=(m_Point+n_Point)/2;    //���õ�2��ֱ��   
	m_Line=FirstLine.BisectorLine(20);//�д���
	m_length=m_Line.LineLength();

	m_TempPoint=m_Line.EndPoint;

	//�н�Ϊ90ʱ���ڶ���ֱ�ߵ��յ�
	if(m_Angle==90)
	{
		n_TempPoint.x=m_Line.BeginPoint.x;
		n_TempPoint.y=m_Line.BeginPoint.y;
		n_TempPoint.z=m_length;//�н�Ϊ90��ʱz��ȡֵ
	    
	}
	else   //�н�Ϊ0��90ʱ���ڶ���ֱ�ߵ��յ�
	{
		m_Angle=m_Angle*3.1415926/180;
		n_TempPoint.x=m_TempPoint.x;
	    n_TempPoint.y=m_TempPoint.y;
	    n_TempPoint.z=m_length*tan(m_Angle); 
	}                                          
	SecondLine.EndPoint=n_TempPoint;
}

//��ֱ����ƽ��Ľ���
TPoint TPlane::PlaneCrossPoint(TLine CrossLine)             
{
    TPoint CrossPoint;   //���潻��
		 
	TVector a_PVector=a_PVector.PointVector(SecondLine.BeginPoint);  //ƽ������
	TVector b_PVector=FirstLine.LineVector();      //ƽ��ĵ�һ��ֱ�ߵķ�������
	TVector c_PVector=SecondLine.LineVector();     //ƽ��ĵ�2��ֱ�ߵķ������� 

	TVector d_LVector=d_LVector.PointVector(CrossLine.BeginPoint);    //ֱ�ߵ����
	TVector e_LVector=CrossLine.LineVector();       //ֱ�ߵķ�������

	TVector m_Temp=b_PVector.XMultiply(c_PVector);
	double s=m_Temp.DotMultiply(a_PVector);
	double r=m_Temp.DotMultiply(d_LVector);
	double t=m_Temp.DotMultiply(e_LVector);
	double m_Value=(s-r)/t;
	return CrossPoint;
      		   
}

//��������ƽ����
TLine TPlane::PlaneCrossLine(TPlane SecondPlane,TPoint m_Start)
{
	TLine m_CrossLine;
    TVector FPlaneVector=this->GetVerticalVetor();
	TVector SPlaneVector=SecondPlane.GetVerticalVetor();

	TVector LVector=FPlaneVector.XMultiply(SPlaneVector);
	double s=LVector.VectorLength();
    LVector.x=LVector.x/s;
	LVector.y=LVector.y/s;
    LVector.z=LVector.z/s;

	if(LVector.z<0)
	{
		LVector.x=-LVector.x;
		LVector.y=-LVector.y;
		LVector.z=-LVector.z;
	}

	m_CrossLine.BeginPoint=m_Start;//PlaneCrossPoint(SecondPlane.SecondLine);  
	m_CrossLine.EndPoint.x=m_CrossLine.BeginPoint.x+LVector.x;
	m_CrossLine.EndPoint.y=m_CrossLine.BeginPoint.y+LVector.y;
	m_CrossLine.EndPoint.z=m_CrossLine.BeginPoint.z+LVector.z;

	return m_CrossLine;
}
//����ƽ����
TLine TPlane::NearPlaneCrossLine(TPlane SecondPlane)
{
	TLine m_CrossLine;
    
	m_CrossLine=PlaneCrossLine(SecondPlane,FirstLine.EndPoint);
	return m_CrossLine;

}
//ֱ����ƽ���λ�������ƽ��0���ཻ1��
int TPlane::CheckLinePosition(TLine m_nLine)
{
	TVector m_PVector,c_LVector,m_Temp;
	c_LVector=m_nLine.LineVector();
	m_PVector=this->GetVerticalVetor();
	double temp;
	temp=c_LVector.DotMultiply(m_PVector);
	if(temp==0)
		return 0;                   //ֱ����ƽ��ƽ��
	else
		return 1;                   //ֱ����ƽ���ཻ                                  
}
//��ƽ��ķ�����
TVector TPlane::GetVerticalVetor()
{
	TVector a_PVector,b_PVector,n_PVector;
	a_PVector=FirstLine.LineVector();                 //ƽ��ĵ�һ��ֱ�ߵķ�������
	b_PVector=SecondLine.LineVector();//ƽ��ĵ�һ��ֱ�ߵķ�������

    n_PVector=a_PVector.XMultiply(b_PVector);
	return n_PVector;
}