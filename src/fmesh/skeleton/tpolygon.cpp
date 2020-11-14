#include "tpolygon.h"

TPolygon::TPolygon()
{
	m_VertexNum=0;
}

TPolygon::TPolygon(TPolygon &temp)
{
	for(int i=0; i<temp.m_VertexNum; i++)
	    m_Polygon[i]=temp.m_Polygon[i];
	m_VertexNum=temp.m_VertexNum;

}

TPolygon::~TPolygon()
{
	m_Polygon.clear();
}

void TPolygon:: SetData(std::vector<TPoint>& n_Polygon)
{
	for(int i=0;i<n_Polygon.size();i++)
	{
		TPoint m_Temp;
		m_Temp.x=(double)n_Polygon[i].x;
		m_Temp.y=(double)n_Polygon[i].y;
		m_Temp.z=0;

		m_Polygon.push_back(m_Temp);
	}
   m_VertexNum=n_Polygon.size();
	
}
TPoint TPolygon::GetPoint(int which)
{
	if(which<0)
		return m_Polygon[m_VertexNum-1];
	else if(which>=m_VertexNum)
		return m_Polygon[0];
	else
		return m_Polygon[which];

}
void TPolygon::operator =(const TPolygon& poly)
{
	m_VertexNum=poly.m_VertexNum;	//顶点个数拷贝
	m_Polygon.clear();
	for(int i=0; i<poly.m_VertexNum; i++)	//复制顶点
	    m_Polygon.push_back(poly.m_Polygon[i]);
}

void TPolygon::Clear()
{
	m_Polygon.clear();
}

//逆时针排序
void TPolygon::SortVertex()
{
	if(0==m_Polygon.size())
	{
		return ;
	}

	TPoint pointwap;
	double s=GetArea();

	if(s>0)	//如果多边形是顺时针的
	{	for(int n=0;n<m_VertexNum/2; n++)
		{
			pointwap=m_Polygon[m_VertexNum-n-1];
			m_Polygon[m_VertexNum-n-1]=m_Polygon[n];
			m_Polygon[n]=pointwap;		
		}
	}
} 

//排序点坐标
void TPolygon::SortAngle()  
{
	std::vector<double> AngleCos;
	double tmp;
	int i;
	for(i=1;i<m_VertexNum;i++)
	{
		tmp=m_Polygon[i].GetCos(m_Polygon[0]);
		AngleCos.push_back(tmp);
	}

	TPoint temp;
	double m;
	for(i=0;i<m_VertexNum;i++)
	{
		for(int j=i+1;j<m_VertexNum;j++)
			if(AngleCos[i]<=AngleCos[j])
			{
				temp=m_Polygon[i+1];
		       	m_Polygon[i+1]=m_Polygon[j+1];
				m_Polygon[j+1]=temp;
				m=AngleCos[i];
				AngleCos[i]=AngleCos[j];
				AngleCos[j]=m;
			}
	}
}

//多边形的面积 
double TPolygon::GetArea()
{
	int n=m_Polygon.size();
	double area=0;

	for(int i=0;i<n;i++)
	{
		area+=0.5*(m_Polygon[i].x-m_Polygon[(i+1+n)%n].x)*(m_Polygon[i].y+m_Polygon[(i+1+n)%n].y);
	}
    return area;
}

bool TPolygon::PointPosition(TPoint m_Point)
{
	//int pnpoly(int nvert, float* vertx, float* verty, float testx, float testy)

		int i, j, c = 0;

		for (i = 0, j = m_VertexNum - 1; i < m_VertexNum; j = i++) {

			if (((m_Polygon[i].y > m_Point.y) != (m_Polygon[j].y > m_Point.y)) &&
				(m_Point.x < (m_Polygon[j].x - m_Polygon[i].x) * (m_Point.y - m_Polygon[i].y) / (m_Polygon[j].y - m_Polygon[i].y) + m_Polygon[i].x))
				c = !c;
		}

		return c;
}
