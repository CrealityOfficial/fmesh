#if !defined(__TPOINTTPOINT_H__)
#define __TPOINTTPOINT_H__
class TPoint  
{
public:
   	TPoint(double x1=0,double y1=0,double z1=0);     
    virtual ~TPoint();                    							
public:
    void operator =(TPoint SecondPoint);       
	TPoint operator -(TPoint SecondPoint);
    bool operator ==(TPoint SecondPoint); 
	TPoint operator * (double m_Value); 
	TPoint operator +(TPoint SecondPoint); 
	TPoint operator / (double m_Value);

	double GetPointDistance(TPoint SecondPoint); //求两点之间的距离

	//两点连线与x轴的夹角
	double GetCos(TPoint p1);

public:
	double x;  
	double y;			
	double z;							

};

#endif // __TPOINTTPOINT_H__
