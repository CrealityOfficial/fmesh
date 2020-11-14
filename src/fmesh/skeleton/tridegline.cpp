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
//设置屋顶的面
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
//初始化数据成员
void TRidegLine::SetDate(TPolygon& m_nPoly,double m_nAngle)
{	
	m_Counter=m_nPoly.m_VertexNum;
	m_Poly=m_nPoly;
	m_Angle=m_nAngle;

	SetRidegPlane();//获取边+中垂线组成的面	
	SetPolyEdge();	//多边形边线
	SetCrossLine(); //相邻线段交线
	SetCrossPoint();  //获得相邻线段交线交点
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

//设置多边形的边
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

//设置相邻面的交线
void TRidegLine::SetCrossLine()
{
	TLine TempLine;
	 for(int i=0;i<m_Counter;i++)
	 {
		TempLine=m_RidegPlane[(i-1+m_Counter)%m_Counter].NearPlaneCrossLine(m_RidegPlane[i]);//与前一个面交集
		m_CrossLine.push_back(TempLine);
	 }
} 

//设置相邻交线的交点
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

	//逆序
	TPolygon  n_Polygon;
	n_Polygon.SetData(m_nPolygon);  //传递多边形的值
	n_Polygon.SortVertex();	//顶点逆时针排序

	SetDate(n_Polygon, 30);	//初始化屋顶对象		
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

//查找最低点
int TRidegLine::FindLowest(TPoint& LowestPnt)
{
	int which=0;	//最低点索引值
	int Num=m_InsectLines.size();	//面交线个数	
	TPoint TempPnt;	//面交线的交点

	for(int i=0; i<Num; i++)
	{
		//求相邻面交线交点,如果两线都是相邻面的交线则不求交点
		if( (m_InsectLines[i].PL_IDa+2)%m_Counter == m_InsectLines[(i+1)%Num].PL_IDb )	
			TempPnt=m_CrossPoint[m_InsectLines[i].PL_IDb];
		else
			TempPnt=m_InsectLines[i].m_Line.ValidInterSect(
						   m_InsectLines[(i+1)%Num].m_Line);
		
		//比较Z坐标大小
		if(TempPnt.x==OVER )	//如果交点无效(不存在或在射线反向线上)
			continue;

		if(0==i || TempPnt.z<LowestPnt.z)
		{
			LowestPnt=TempPnt;
			which=i;
			which=FindUncommon(LowestPnt,i);	//处理特殊情况
		}			
	}
	return which;
}

//查找最低点特殊情况
int TRidegLine::FindUncommon(TPoint& LowestPnt,int& index)
{
	int Num=m_InsectLines.size();
	int pos=index;	//记录返回值
	//如果面交线方向朝下
	if(m_InsectLines[index].m_Line.BeginPoint.z >
		m_InsectLines[index].m_Line.EndPoint.z )
	{
	
		TPoint LCutPoint;	//线被左邻线切割的点
		LCutPoint=m_InsectLines[index].m_Line.ValidInterSect(
					 m_InsectLines[(index-1+Num)%Num].m_Line);
		
		//取较近(高)的点为有效点
		if(LCutPoint.z != OVER && (LCutPoint.z > LowestPnt.z))
		{
			LowestPnt=LCutPoint;
			pos=index-1;
		}
	}
	return pos;
}

//构造中轴线
void TRidegLine::CreateAxesLine()
{
	//生成初始的面交线链表
	CInsectLine ILine;	//相邻面交线
	for(int i=0; i<m_Counter; i++)
	{	
		ILine.m_Line=m_CrossLine[i];
		ILine.PL_IDa=(i-1+m_Counter)%m_Counter;
		ILine.PL_IDb=i;
		m_InsectLines.push_back(ILine);
	}

	m_RidegLine.clear();

	//循环生成中轴线
	TPoint tPnt;	//最低交点
	int Position;	//最低点在链表中的序号

	for(int i=0; i<m_Counter-2; i++)
	{
		Position=FindLowest(tPnt);	//查找最低点

		if (m_Poly.PointPosition(tPnt))
		{
			AddNewAxes(Position, tPnt);	//插入两条与最低点相关的面交线
		}

		UpdateILines(Position,tPnt);	//更新面交线链表	
	}
	//最后一条
	m_RidegLine.push_back(TLine(m_InsectLines[0].m_Line.BeginPoint,m_InsectLines[1].m_Line.BeginPoint));
	
}

//插入两条与最低点相关的面交线
void TRidegLine::AddNewAxes(int Position,TPoint tPnt)
{
	TLine LineA=m_InsectLines[Position].m_Line;
	TLine LineB=m_InsectLines[(Position+1)%m_InsectLines.size()].m_Line;
	LineA.EndPoint=tPnt;	//更改两直线终点
	LineB.EndPoint=tPnt;
	m_AxesPoints.push_back(tPnt);	//插入骨架结点
	m_RidegLine.push_back(LineA);	//将两线插入中轴线链表
	m_RidegLine.push_back(LineB);
}

//更新面交线链表
void TRidegLine::UpdateILines(int Position,TPoint tPnt)
{
	int a=m_InsectLines[Position].PL_IDa;
	int b=m_InsectLines[(Position+1)%m_InsectLines.size()].PL_IDb;

	CInsectLine ILine;	//创建新的面交线
	ILine.m_Line=m_RidegPlane[a].PlaneCrossLine(m_RidegPlane[b],tPnt);
	ILine.PL_IDa=a;
	ILine.PL_IDb=b;

	m_InsectLines.insert(m_InsectLines.begin()+ Position, ILine);	//插入新的面交线
	int index;	//旧面交线的位置
	for(int i=0; i<2; i++)	//删除两条旧的面交线
	{
		index=(Position+1)%m_InsectLines.size();
		m_InsectLines.erase(m_InsectLines.begin() + index);
	}
}