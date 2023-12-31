#include "svg.h"
#include <iostream>
#include "map"
#include <sstream>

#define SCALE1000(n) (int64_t((n) * 1000))
#define  SVG_SVG "svg"//svg视图大小
#define  SVG_LINE "line"//线段
#define  SVG_POLYLINE "polyline"//线段
#define  SVG_RECT "rect"//矩形
#define  SVG_CIRCLE "circle"//圆
#define  SVG_ELLIPSE "ellipse"//椭圆
#define  SVG_POLYGON "polygon"//多边形
#define  SVG_PATH "path"//路径
#define  SVG_G "g"//组

double const PI = 3.141592653589793238;


//模板函数：将string类型变量转换为常用的数值类型
template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}


svg::svg()
{
	m_paths = new ClipperLibXYZ::Paths();
	m_BezierPoint.first = false;
	m_BezierPoint.second = ClipperLibXYZ::DoublePoint(0, 0);
	m_scaleX = 1.0;
	m_scaleY = 1.0;
	m_currentMatrix3x3[3][3] = { 0 };
}

svg::~svg()
{

}

void svg::pauseSVGEX(TiXmlElement* root)
{
	for (TiXmlElement* tmpele = root->FirstChildElement(); tmpele; tmpele = tmpele->NextSiblingElement())
	{
		TiXmlElement* currentElement = tmpele;
		string strElemValue = currentElement->Value();
		if (strElemValue == SVG_SVG)
		{
			vector<string>vctBox;
			if (currentElement->Attribute("viewBox") && currentElement->Attribute("width") && currentElement->Attribute("height"))
			{
				string strWidth = currentElement->Attribute("width");//SVG文件的宽度
				string strHeight = currentElement->Attribute("height");//SVG文件的高度
				float iWidth = atof(strWidth.data());//字体宽度
				float iHeight = atof(strHeight.data());//字体高度
				string strViewBox = currentElement->Attribute("viewBox");//视图
				SplitString(strViewBox, vctBox, " ");
				if (4 == vctBox.size())
				{
					float ViewBoxStartX = atof(vctBox[0].data());//ViewBox的起点X
					float ViewBoxStartY = atof(vctBox[1].data());//ViewBox的起点Y
					float ViewBoxWidth = atof(vctBox[2].data()) - ViewBoxStartX; //ViewBox的宽度
					float ViewBoxHeight = atof(vctBox[3].data()) - ViewBoxStartY;//ViewBox的高度

					m_scaleX = (iWidth - ViewBoxStartX) / ViewBoxWidth;
					m_scaleY = (iHeight - ViewBoxStartY) / ViewBoxHeight;
				}
			}
			else
			{
				m_scaleX = 1.0;
				m_scaleY = 1.0;
			}
		}
		else if (strElemValue == SVG_G)
		{
			vector<string> vctNum;
			if (currentElement->Attribute("transform"))
			{
				string strWidth = currentElement->Attribute("transform");//偏移矩阵
				int pos = strWidth.find("matrix(");
				if (pos != string::npos)
				{
					strWidth = strWidth.substr(7, strWidth.length() - 7);
				}
				pos = strWidth.find(")");
				if (pos != string::npos)
				{
					strWidth = strWidth.substr(0, strWidth.length() - 1);
				}

				SplitString(strWidth, vctNum, ",");
			}
			if (vctNum.size() == 6)
			{
				m_currentMatrix3x3[0][0] = stringToNum<float>(vctNum[0]);
				m_currentMatrix3x3[0][1] = stringToNum<float>(vctNum[2]);
				m_currentMatrix3x3[0][2] = stringToNum<float>(vctNum[4]);

				m_currentMatrix3x3[1][0] = stringToNum<float>(vctNum[1]);
				m_currentMatrix3x3[1][1] = stringToNum<float>(vctNum[3]);
				m_currentMatrix3x3[1][2] = stringToNum<float>(vctNum[5]);

				m_currentMatrix3x3[2][0] = 0;
				m_currentMatrix3x3[2][1] = 0;
				m_currentMatrix3x3[2][2] = 1;
			}
			else
			{
				m_currentMatrix3x3[0][0] = 1;
				m_currentMatrix3x3[0][1] = 0;
				m_currentMatrix3x3[0][2] = 0;

				m_currentMatrix3x3[1][0] = 0;
				m_currentMatrix3x3[1][1] = 1;
				m_currentMatrix3x3[1][2] = 0;

				m_currentMatrix3x3[2][0] = 0;
				m_currentMatrix3x3[2][1] = 0;
				m_currentMatrix3x3[2][2] = 1;
			}
		}
		else if (strElemValue == SVG_LINE)
		{
			float start_x = atof(currentElement->Attribute("x1"));
			float start_y = atof(currentElement->Attribute("y1"));
			float end_x = atof(currentElement->Attribute("x2"));
			float end_y = atof(currentElement->Attribute("y2"));
			//处理线段
			pauseLine(start_x, start_y, end_x, end_y);
		}
		else if (strElemValue == SVG_POLYLINE)
		{
			string strPoints = currentElement->Attribute("points");
			//处理多线段
			pausePolyline(strPoints);
		}
		else if (strElemValue == SVG_RECT)
		{
			float width = atof(currentElement->Attribute("width"));
			float height = atof(currentElement->Attribute("height"));
			float leftTopX = atof(currentElement->Attribute("x"));
			float leftTopY = atof(currentElement->Attribute("y"));
			//处理矩形
			pauseRect(width, height, leftTopX, leftTopY);
		}
		else if (strElemValue == SVG_CIRCLE)
		{
			float cx = atof(currentElement->Attribute("cx"));
			float cy = atof(currentElement->Attribute("cy"));
			float rr = atof(currentElement->Attribute("r"));
			//处理圆
			pauseEllipse(cx, cy, rr, rr);
		}
		else if (strElemValue == SVG_ELLIPSE)
		{
			float cx = atof(currentElement->Attribute("cx"));
			float cy = atof(currentElement->Attribute("cy"));
			float rx = atof(currentElement->Attribute("rx"));
			float ry = atof(currentElement->Attribute("ry"));
			//处理椭圆
			pauseEllipse(cx, cy, rx, ry);
		}
		else if (strElemValue == SVG_POLYGON)
		{
			string strPoints = currentElement->Attribute("points");
			//处理多边形
			pausePolygon(strPoints);
		}
		else if (strElemValue == SVG_PATH)
		{
			string strPath = currentElement->Attribute("d");
			//处理path文件
			pausePath(strPath);
		}

		//递归遍历
		if (!currentElement->NoChildren())
			pauseSVGEX(currentElement);

	}
}




void svg::pauseSVG(TiXmlElement* root)
{
	TiXmlElement* elemBrother = root;
	while (elemBrother)
	{
		string strElemValue = elemBrother->Value();
		if (strElemValue == SVG_SVG)
		{
			vector<string>vctBox;
			if (elemBrother->Attribute("viewBox")&& elemBrother->Attribute("width")&& elemBrother->Attribute("height"))
			{
				string strWidth = elemBrother->Attribute("width");//SVG文件的宽度
				string strHeight = elemBrother->Attribute("height");//SVG文件的高度
				float iWidth = atof(strWidth.data());//字体宽度
				float iHeight = atof(strHeight.data());//字体高度
				string strViewBox = elemBrother->Attribute("viewBox");//视图
				SplitString(strViewBox, vctBox, " ");
				if (4 == vctBox.size())
				{
					float ViewBoxStartX = atof(vctBox[0].data());//ViewBox的起点X
					float ViewBoxStartY = atof(vctBox[1].data());//ViewBox的起点Y
					float ViewBoxWidth = atof(vctBox[2].data()) - ViewBoxStartX; //ViewBox的宽度
					float ViewBoxHeight = atof(vctBox[3].data()) - ViewBoxStartY;//ViewBox的高度

					m_scaleX = (iWidth - ViewBoxStartX) / ViewBoxWidth;
					m_scaleY = (iHeight - ViewBoxStartY) / ViewBoxHeight;
				}
			}
			else
			{
				m_scaleX = 1.0;
				m_scaleY = 1.0;
			}
		} 
		else if (strElemValue == SVG_G)
		{
			vector<string> vctNum;
			if (elemBrother->Attribute("transform"))
			{
				string strWidth = elemBrother->Attribute("transform");//偏移矩阵
				int pos = strWidth.find("matrix(");
				if (pos!= string::npos)
				{
					strWidth = strWidth.substr(7, strWidth.length()-7);
				}
				pos = strWidth.find(")");
				if (pos != string::npos)
				{
					strWidth = strWidth.substr(0, strWidth.length() - 1);
				}

				SplitString(strWidth, vctNum,",");
			}
			if (vctNum.size() == 6)
			{
				m_currentMatrix3x3[0][0] = stringToNum<float>(vctNum[0]);
				m_currentMatrix3x3[0][1] = stringToNum<float>(vctNum[2]);
				m_currentMatrix3x3[0][2] = stringToNum<float>(vctNum[4]);

				m_currentMatrix3x3[1][0] = stringToNum<float>(vctNum[1]);
				m_currentMatrix3x3[1][1] = stringToNum<float>(vctNum[3]);
				m_currentMatrix3x3[1][2] = stringToNum<float>(vctNum[5]);

				m_currentMatrix3x3[2][0] = 0;
				m_currentMatrix3x3[2][1] = 0;
				m_currentMatrix3x3[2][2] = 1;
			}
			else
			{
				m_currentMatrix3x3[0][0] = 1;
				m_currentMatrix3x3[0][1] = 0;
				m_currentMatrix3x3[0][2] = 0;

				m_currentMatrix3x3[1][0] = 0;
				m_currentMatrix3x3[1][1] = 1;
				m_currentMatrix3x3[1][2] = 0;

				m_currentMatrix3x3[1][0] = 0;
				m_currentMatrix3x3[1][1] = 0;
				m_currentMatrix3x3[1][2] = 1;
			}
		}
		else if (strElemValue == SVG_LINE)
		{
			float start_x = atof(elemBrother->Attribute("x1"));
			float start_y = atof(elemBrother->Attribute("y1"));
			float end_x = atof(elemBrother->Attribute("x2"));
			float end_y = atof(elemBrother->Attribute("y2"));
			//处理线段
			pauseLine(start_x, start_y, end_x, end_y);
		}
		else if (strElemValue == SVG_POLYLINE)
		{
			string strPoints = elemBrother->Attribute("points");
			//处理多线段
			pausePolyline(strPoints);
		}
		else if (strElemValue == SVG_RECT)
		{
			float width = atof(elemBrother->Attribute("width"));
			float height = atof(elemBrother->Attribute("height"));
			float leftTopX = atof(elemBrother->Attribute("x"));
			float leftTopY = atof(elemBrother->Attribute("y"));
			//处理矩形
			pauseRect(width, height, leftTopX, leftTopY);
		}
		else if (strElemValue == SVG_CIRCLE)
		{
			float cx = atof(elemBrother->Attribute("cx"));
			float cy = atof(elemBrother->Attribute("cy"));
			float rr = atof(elemBrother->Attribute("r"));
			//处理圆
			pauseEllipse(cx, cy, rr, rr);
		}
		else if (strElemValue == SVG_ELLIPSE)
		{
			float cx = atof(elemBrother->Attribute("cx"));
			float cy = atof(elemBrother->Attribute("cy"));
			float rx = atof(elemBrother->Attribute("rx"));
			float ry = atof(elemBrother->Attribute("ry"));
			//处理椭圆
			pauseEllipse(cx, cy, rx, ry);
		}
		else if (strElemValue == SVG_POLYGON)
		{
			string strPoints = elemBrother->Attribute("points");
			//处理多边形
			pausePolygon(strPoints);
		}
		else if (strElemValue == SVG_PATH)
		{
			string strPath = elemBrother->Attribute("d");
			//处理path文件
			pausePath(strPath);
		}

		//递归
		elemBrother = elemBrother->NextSiblingElement();//获取下一个兄弟节点
		if (elemBrother == NULL)
		{
			for (; ; root = root->NextSiblingElement())//如果当前节点下面没有节点
			{
				if (root == NULL)
					break;
				if (std::string(root->Value()) == "defs")
					continue;
				if (std::string(root->Value()) == "missing-glyph")
					continue;
				if (root->FirstChildElement() != NULL)
				{
					pauseSVG(root->FirstChildElement());//获取当前兄弟节点的子节点
				}
			}
		}
	}
}



void svg::pauseRect(float width, float height, float leftTopX, float leftTopY)
{
	vector<ClipperLibXYZ::DoublePoint> vctDoublePoint;
	ClipperLibXYZ::DoublePoint PointTopLeft;
	ClipperLibXYZ::DoublePoint PointTopRight;
	ClipperLibXYZ::DoublePoint PointBottomLeft;
	ClipperLibXYZ::DoublePoint PointBottomRight;

	PointTopLeft.X = leftTopX* m_scaleX;
	PointTopLeft.Y = leftTopY * m_scaleY;
	vctDoublePoint.push_back(PointTopLeft);

	PointTopRight.X = (leftTopX + width) * m_scaleX;
	PointTopRight.Y = leftTopY * m_scaleY;
	vctDoublePoint.push_back(PointTopRight);

	PointBottomRight.X = (leftTopX + width) * m_scaleX;
	PointBottomRight.Y = (leftTopY + height) * m_scaleY;
	vctDoublePoint.push_back(PointBottomRight);

	PointBottomLeft.X = leftTopX * m_scaleX;
	PointBottomLeft.Y = (leftTopY + height) * m_scaleY;
	vctDoublePoint.push_back(PointBottomLeft);

	m_paths->push_back(DoublePoint2IntPoint(vctDoublePoint));
}

void svg::pauseLine(float x1, float y1, float x2, float y2)
{
	vector<ClipperLibXYZ::DoublePoint> vctDoublePoint;
	ClipperLibXYZ::DoublePoint startPoint;
	ClipperLibXYZ::DoublePoint endPoint;
	startPoint.X = x1;
	startPoint.Y = y1;
	endPoint.X = x2;
	endPoint.Y = y2;
	vctDoublePoint.push_back(startPoint);
	vctDoublePoint.push_back(endPoint);
	m_paths->push_back(DoublePoint2IntPoint(vctDoublePoint));
}

void svg::pauseEllipse(float Cx, float Cy, float Rx, float Ry, float precision)
{
	vector<ClipperLibXYZ::DoublePoint> vctDoublePoint;
	for (unsigned int i = 0; i < precision; i++)//顺时针取点
	{
		double Angle = 2*PI * (i / precision);
		size_t nSize = vctDoublePoint.size();
		vctDoublePoint.emplace_back();
		vctDoublePoint[nSize].X = (Cx + Rx * cos(Angle)) * m_scaleX;
		vctDoublePoint[nSize].Y = (Cy + Ry * sin(Angle)) * m_scaleY;
	}
	m_paths->push_back(DoublePoint2IntPoint(vctDoublePoint));
}

void svg::pausePolyline(string strPoints)
{
	vector<ClipperLibXYZ::DoublePoint> vctDoublePoint;
	vector<std::string> VctPoint;
	vector<std::string> VctS = { " ","," };
	SplitStringS(strPoints, VctPoint, VctS);
	for (int n = 1; n < VctPoint.size(); n += 2)
	{
		size_t nSize = vctDoublePoint.size();
		vctDoublePoint.emplace_back();
		vctDoublePoint[nSize].X = atof(VctPoint[n - 1].data()) * m_scaleX;
		vctDoublePoint[nSize].Y = atof(VctPoint[n].data()) * m_scaleY;
	}
	m_currentPosition = vctDoublePoint[vctDoublePoint.size() - 1];
	m_paths->push_back(DoublePoint2IntPoint(vctDoublePoint));
}

void svg::pausePolygon(string strPoints)
{
	vector<ClipperLibXYZ::DoublePoint> vctDoublePoint;
	std::vector<std::string> Vctdest;
	SplitString(strPoints, Vctdest," ");
	for (std::string strTemp : Vctdest)
	{
		std::vector<std::string> VctPoint;
		SplitString(strTemp, VctPoint, ",");
		if (2 == VctPoint.size())
		{
			size_t nSize = vctDoublePoint.size();
			vctDoublePoint.emplace_back();
			vctDoublePoint[nSize].X = atof(VctPoint[0].data())* m_scaleX;
			vctDoublePoint[nSize].Y = atof(VctPoint[1].data())* m_scaleY;
		}
	}
	m_paths->push_back(DoublePoint2IntPoint(vctDoublePoint));
}

void svg::pausePath(string strPath)
{
	vector<ClipperLibXYZ::DoublePoint> vctDoublePoint;
	vector<string> vctString;
	bool flagBegin = false;
	string strTemp;
	for (int n=0;n<strPath.size();n++)
	{
		if(isSpechars(strPath[n]))
		{
			if (flagBegin == false)
			{
				strTemp += strPath[n];
				flagBegin = true;
			}
			else
			{
				vctString.push_back(strTemp);
				strTemp.clear();
				flagBegin = false;
				n--;
			}
		}
		else 
		{ 
			if (flagBegin) strTemp += strPath[n];
		}

		//最后一个
		if (n == strPath.size()-1)
		{
			vctString.push_back(strTemp);
		}
	}

	for (string stringPoint: vctString)
	{
		switch(stringPoint[0])
		{
			case 'm':
			case 'M':
			{	
				pausePathMm(stringPoint, vctDoublePoint);
				break;
			}
			case 'L':
			case 'l':
			{
				pausePathLl(stringPoint, vctDoublePoint);
				break;
			}
			case 'H':
			case 'h':
			{
				pausePathHh(stringPoint, vctDoublePoint);
				break;
			}
			case 'v':
			case 'V':
			{
				pausePathVv(stringPoint, vctDoublePoint);
				break;
			}
			case 'C':
			case 'c':
			{
				pausePathCc(stringPoint, vctDoublePoint);
				break;
			}
			case 'Q':
			case 'q':
			{
				pausePathQq(stringPoint, vctDoublePoint);
				break;
			}
			case 'T':
			case 't':
			{
				pausePathTt(stringPoint, vctDoublePoint);
				break;
			}
			case 'S':
			case 's':
			{
				pausePathSs(stringPoint, vctDoublePoint);
				break;
			}
			case 'A':
			case 'a':
			{
				pausePathAa(stringPoint, vctDoublePoint);
				break;
			}
			case 'Z':
			case 'z':
			{
				m_paths->push_back(DoublePoint2IntPoint(vctDoublePoint));
				vctDoublePoint.clear();
				break;
			}
			default:
			{
				savePenultPoint();
				break;
			}
		}//end switch
	}
	if (vctDoublePoint.size())
	{
		m_paths->push_back(DoublePoint2IntPoint(vctDoublePoint));
		vctDoublePoint.clear();
	}
}

void svg::pausePathMm(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint)
{
	if (stringPoint.substr(0, 1) == "M")
	{
		m_isBigchars = true;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'M'), stringPoint.end());
//#endif
	}
	else
	{
		m_isBigchars = false;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'm'), stringPoint.end());
//#endif
	}

	vector<std::string> VctPoint;
	vector<std::string> VctS = {" ",","};
	SplitStringS(stringPoint, VctPoint, VctS);
	for (int n=1;n< VctPoint.size();n+=2)
	{
		size_t nSize = vctDoublePoint.size();
		vctDoublePoint.emplace_back();
		if (m_isBigchars)
		{
			vctDoublePoint[nSize].X = atof(VctPoint[n-1].data()) * m_scaleX;
			vctDoublePoint[nSize].Y = -atof(VctPoint[n].data()) * m_scaleY;
		}
		else
		{
			vctDoublePoint[nSize].X = m_currentPosition.X + atof(VctPoint[n-1].data()) * m_scaleX;
			vctDoublePoint[nSize].Y = m_currentPosition.Y - atof(VctPoint[n].data()) * m_scaleY;
		}
		m_currentPosition = vctDoublePoint[nSize];
	}
	savePenultPoint();
}

void svg::pausePathLl(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint)
{
	if (stringPoint.substr(0, 1) == "L")
	{
		m_isBigchars = true;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'L'), stringPoint.end());
//#endif
	}
	else
	{
		m_isBigchars = false;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'l'), stringPoint.end());
//#endif
	}

	vector<std::string> VctPoint;
	vector<std::string> VctS = { " ","," };
	SplitStringS(stringPoint, VctPoint, VctS);
	for (int n = 1; n < VctPoint.size(); n += 2)
	{
		size_t nSize = vctDoublePoint.size();
		vctDoublePoint.emplace_back();
		if (m_isBigchars)
		{
			vctDoublePoint[nSize].X = atof(VctPoint[n - 1].data()) * m_scaleX;
			vctDoublePoint[nSize].Y = -atof(VctPoint[n].data()) * m_scaleY;
		}
		else
		{
			vctDoublePoint[nSize].X = m_currentPosition.X + atof(VctPoint[n - 1].data()) * m_scaleX;
			vctDoublePoint[nSize].Y = m_currentPosition.Y - atof(VctPoint[n].data()) * m_scaleY;
		}
		m_currentPosition = vctDoublePoint[vctDoublePoint.size() - 1];
	}
	savePenultPoint();
}

void svg::pausePathHh(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint)
{
	if (stringPoint.substr(0, 1) == "H")
	{
		m_isBigchars = true;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'H'), stringPoint.end());
//#endif
	}
	else
	{
		m_isBigchars = false;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'h'), stringPoint.end());
//#endif
	}

	vector<std::string> VctPoint;
	SplitString(stringPoint, VctPoint, " ");
	if (1 == VctPoint.size())
	{
		size_t nSize = vctDoublePoint.size();
		vctDoublePoint.emplace_back();
		if (m_isBigchars)
		{
			vctDoublePoint[nSize].X = atof(VctPoint[0].data()) * m_scaleX;
			vctDoublePoint[nSize].Y = m_currentPosition.Y;
		}
		else
		{
			vctDoublePoint[nSize].X = m_currentPosition.X + atof(VctPoint[0].data()) * m_scaleX;
			vctDoublePoint[nSize].Y = m_currentPosition.Y;
		}
		m_currentPosition = vctDoublePoint[nSize];
	}
	savePenultPoint();
}

void svg::pausePathVv(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint)
{
	if (stringPoint.substr(0, 1) == "V")
	{
		m_isBigchars = true;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'V'), stringPoint.end());
//#endif
	}
	else
	{
		m_isBigchars = false;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'v'), stringPoint.end());
//#endif
	}

	vector<std::string> VctPoint;
	SplitString(stringPoint, VctPoint, " ");
	if (1 == VctPoint.size())
	{
		size_t nSize = vctDoublePoint.size();
		vctDoublePoint.emplace_back();
		if (m_isBigchars)
		{
			vctDoublePoint[nSize].X = m_currentPosition.X;
			vctDoublePoint[nSize].Y = atof(VctPoint[0].data()) * m_scaleY;
		}
		else
		{
			vctDoublePoint[nSize].X = m_currentPosition.X;
			vctDoublePoint[nSize].Y = m_currentPosition.Y + atof(VctPoint[0].data()) * m_scaleY;
		}
		m_currentPosition = vctDoublePoint[nSize];
	}
	savePenultPoint();
}

void svg::pausePathCc(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint)
{
	if (stringPoint.substr(0, 1) == "C")
	{
		m_isBigchars = true;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'C'), stringPoint.end());
//#endif
	}
	else
	{
		m_isBigchars = false;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'c'), stringPoint.end());
//#endif
	}

	vector<std::string> VctPoint;
	vector<std::string> VctS = { " ","," };
	SplitStringS(stringPoint, VctPoint, VctS);
	for (int n=5; n < VctPoint.size(); n += 6)
	{
		ClipperLibXYZ::DoublePoint bezierStart = m_currentPosition;
		if (m_isBigchars)
		{
			vector<ClipperLibXYZ::DoublePoint> vctBasePoint;
			vctBasePoint.push_back(m_currentPosition);
			m_currentPosition.X = atof(VctPoint[n-5].data()) * m_scaleX;
			m_currentPosition.Y = -atof(VctPoint[n-4].data()) * m_scaleY;
			vctBasePoint.push_back(m_currentPosition);
			m_currentPosition.X = atof(VctPoint[n-3].data()) * m_scaleX;
			m_currentPosition.Y = -atof(VctPoint[n-2].data()) * m_scaleY;
			vctBasePoint.push_back(m_currentPosition);
			m_currentPosition.X = atof(VctPoint[n-1].data()) * m_scaleX;
			m_currentPosition.Y = -atof(VctPoint[n].data()) * m_scaleY;
			vctBasePoint.push_back(m_currentPosition);
			for (int tm = 1; tm <= 50; tm++)
			{
				ClipperLibXYZ::DoublePoint FT_Temp;
				float t = tm / 50.0;
				FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 3) + vctBasePoint[1].X * t * pow(1 - t, 2) * 3 + vctBasePoint[2].X * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].X * pow(t, 3);
				FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 3) + vctBasePoint[1].Y * t * pow(1 - t, 2) * 3 + vctBasePoint[2].Y * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].Y * pow(t, 3);
				vctDoublePoint.push_back(FT_Temp);
			}
			savePenultPoint(vctBasePoint[2], vctBasePoint[3]);
			vctBasePoint.clear();
		}
		else
		{
			vector<ClipperLibXYZ::DoublePoint> vctBasePoint;
			vctBasePoint.push_back(m_currentPosition);
			m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 5].data()) * m_scaleX;
			m_currentPosition.Y = bezierStart.Y -atof(VctPoint[n - 4].data()) * m_scaleY;
			vctBasePoint.push_back(m_currentPosition);
			m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 3].data()) * m_scaleX;
			m_currentPosition.Y = bezierStart.Y -atof(VctPoint[n - 2].data()) * m_scaleY;
			vctBasePoint.push_back(m_currentPosition);
			m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data()) * m_scaleX;
			m_currentPosition.Y = bezierStart.Y -atof(VctPoint[n].data()) * m_scaleY;
			vctBasePoint.push_back(m_currentPosition);
			for (int tm = 1; tm <= 50; tm++)
			{
				ClipperLibXYZ::DoublePoint FT_Temp;
				float t = tm / 50.0;
				FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 3) + vctBasePoint[1].X * t * pow(1 - t, 2) * 3 + vctBasePoint[2].X * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].X * pow(t, 3);
				FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 3) + vctBasePoint[1].Y * t * pow(1 - t, 2) * 3 + vctBasePoint[2].Y * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].Y * pow(t, 3);
				vctDoublePoint.push_back(FT_Temp);
			}
			savePenultPoint(vctBasePoint[2], vctBasePoint[3]);
			vctBasePoint.clear();
		}
		m_currentPosition = vctDoublePoint[vctDoublePoint.size() - 1];
	}
}

void svg::pausePathQq(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint)
{
	if (stringPoint.substr(0, 1) == "Q")
	{
		m_isBigchars = true;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'Q'), stringPoint.end());
//#endif
	}
	else
	{
		m_isBigchars = false;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'q'), stringPoint.end());
//#endif
	}

	vector<std::string> VctPoint;
	vector<std::string> VctS = { " ","," };
	SplitStringS(stringPoint, VctPoint, VctS);

	for (int n =3;n< VctPoint.size();n+=4)
	{
		if (m_isBigchars)
		{
			vector<ClipperLibXYZ::DoublePoint> vctBasePoint;
			vctBasePoint.push_back(m_currentPosition);
			m_currentPosition.X = atof(VctPoint[n-3].data()) * m_scaleX;
			m_currentPosition.Y = -atof(VctPoint[n-2].data()) * m_scaleY;
			vctBasePoint.push_back(m_currentPosition);
			m_currentPosition.X = atof(VctPoint[n-1].data()) * m_scaleX;
			m_currentPosition.Y = -atof(VctPoint[n].data()) * m_scaleY;
			vctBasePoint.push_back(m_currentPosition);
			for (int tm = 1; tm <= 50; tm++)
			{
				ClipperLibXYZ::DoublePoint FT_Temp;
				float t = tm / 50.0;
				FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].X + pow(t, 2) * vctBasePoint[2].X;
				FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].Y + pow(t, 2) * vctBasePoint[2].Y;
				vctDoublePoint.push_back(FT_Temp);
			}
			savePenultPoint(vctBasePoint[1], vctBasePoint[2]);
			vctBasePoint.clear();
		}
		else
		{
			ClipperLibXYZ::DoublePoint bezierStart = m_currentPosition;
			vector<ClipperLibXYZ::DoublePoint> vctBasePoint;
			vctBasePoint.push_back(m_currentPosition);
			m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 3].data()) * m_scaleX;
			m_currentPosition.Y = bezierStart.Y -atof(VctPoint[n - 2].data()) * m_scaleY;
			vctBasePoint.push_back(m_currentPosition);
			m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data()) * m_scaleX;
			m_currentPosition.Y = bezierStart.Y -atof(VctPoint[n].data()) * m_scaleY;
			vctBasePoint.push_back(m_currentPosition);
			for (int tm = 1; tm <= 50; tm++)
			{
				ClipperLibXYZ::DoublePoint FT_Temp;
				float t = tm / 50.0;
				FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].X + pow(t, 2) * vctBasePoint[2].X;
				FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].Y + pow(t, 2) * vctBasePoint[2].Y;
				vctDoublePoint.push_back(FT_Temp);
			}
			savePenultPoint(vctBasePoint[1], vctBasePoint[2]);
			vctBasePoint.clear();
		}
		m_currentPosition = vctDoublePoint[vctDoublePoint.size() - 1];
	}
}

void svg::pausePathTt(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint)
{
	if (stringPoint.substr(0, 1) == "T")
	{
		m_isBigchars = true;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'T'), stringPoint.end());
//#endif
	}
	else
	{
		m_isBigchars = false;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 't'), stringPoint.end());
//#endif
	}

	vector<std::string> VctPoint;
	vector<std::string> VctS = { " ","," };
	SplitStringS(stringPoint, VctPoint, VctS);
	for (int n = 1; n < VctPoint.size(); n += 2)
	{
		ClipperLibXYZ::DoublePoint bezierStart = m_currentPosition;
		if (m_isBigchars)
		{
			vector<ClipperLibXYZ::DoublePoint> vctBasePoint;
			if (m_BezierPoint.first)
			{
				vctBasePoint.push_back(m_currentPosition);
				vctBasePoint.push_back(m_BezierPoint.second);
				m_currentPosition.X = atof(VctPoint[n - 1].data()) * m_scaleX;
				m_currentPosition.Y = -atof(VctPoint[n].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);
			}
			else
			{
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = atof(VctPoint[n - 1].data()) * m_scaleX;
				m_currentPosition.Y = -atof(VctPoint[n].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);
				vctBasePoint.push_back(m_currentPosition);
			}
			for (int tm = 1; tm <= 50; tm++)
			{
				ClipperLibXYZ::DoublePoint FT_Temp;
				float t = tm / 50.0;
				FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].X + pow(t, 2) * vctBasePoint[2].X;
				FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].Y + pow(t, 2) * vctBasePoint[2].Y;
				vctDoublePoint.push_back(FT_Temp);
			}
			savePenultPoint(vctBasePoint[1], vctBasePoint[2]);
			vctBasePoint.clear();
		}
		else
		{
			ClipperLibXYZ::DoublePoint bezierStart = m_currentPosition;
			vector<ClipperLibXYZ::DoublePoint> vctBasePoint;
			if (m_BezierPoint.first)
			{
				vctBasePoint.push_back(m_currentPosition);
				vctBasePoint.push_back(m_BezierPoint.second);
				m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data()) * m_scaleX;
				m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);
			}
			else
			{
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data()) * m_scaleX;
				m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);
				vctBasePoint.push_back(m_currentPosition);
			}
			for (int tm = 1; tm <= 50; tm++)
			{
				ClipperLibXYZ::DoublePoint FT_Temp;
				float t = tm / 50.0;
				FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].X + pow(t, 2) * vctBasePoint[2].X;
				FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].Y + pow(t, 2) * vctBasePoint[2].Y;
				vctDoublePoint.push_back(FT_Temp);
			}
			savePenultPoint(vctBasePoint[1], vctBasePoint[2]);
			vctBasePoint.clear();
		}
		m_currentPosition = vctDoublePoint[vctDoublePoint.size() - 1];
	}
}

void svg::pausePathSs(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint)
{
	if (stringPoint.substr(0, 1) == "S")
	{
		m_isBigchars = true;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 'S'), stringPoint.end());
//#endif
	}
	else
	{
		m_isBigchars = false;
		stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
//#if WIN32
//		stringPoint.erase(std::remove(stringPoint.begin(), stringPoint.end(), 's'), stringPoint.end());
//#endif
	}

	vector<std::string> VctPoint;
	vector<std::string> VctS = { " ","," };
	SplitStringS(stringPoint, VctPoint, VctS);
	for (int n = 3; n < VctPoint.size(); n += 4)
	{
		ClipperLibXYZ::DoublePoint bezierStart = m_currentPosition;
		if (m_isBigchars)
		{
			vector<ClipperLibXYZ::DoublePoint> vctBasePoint;
			if (m_BezierPoint.first)
			{
				vctBasePoint.push_back(m_currentPosition);
				vctBasePoint.push_back(m_BezierPoint.second);
				m_currentPosition.X = atof(VctPoint[n - 3].data()) * m_scaleX;
				m_currentPosition.Y = -atof(VctPoint[n - 2].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = atof(VctPoint[n - 1].data()) * m_scaleX;
				m_currentPosition.Y = -atof(VctPoint[n].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);
			}
			else
			{
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = atof(VctPoint[n - 3].data()) * m_scaleX;
				m_currentPosition.Y = -atof(VctPoint[n - 2].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = atof(VctPoint[n - 1].data()) * m_scaleX;
				m_currentPosition.Y = -atof(VctPoint[n].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);

			}
			for (int tm = 1; tm <= 50; tm++)
			{
				ClipperLibXYZ::DoublePoint FT_Temp;
				float t = tm / 50.0;
				FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 3) + vctBasePoint[1].X * t * pow(1 - t, 2) * 3 + vctBasePoint[2].X * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].X * pow(t, 3);
				FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 3) + vctBasePoint[1].Y * t * pow(1 - t, 2) * 3 + vctBasePoint[2].Y * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].Y * pow(t, 3);
				vctDoublePoint.push_back(FT_Temp);
			}
			savePenultPoint(vctBasePoint[2], vctBasePoint[3]);
			vctBasePoint.clear();
		}
		else
		{
			ClipperLibXYZ::DoublePoint bezierStart = m_currentPosition;
			vector<ClipperLibXYZ::DoublePoint> vctBasePoint;
			if (m_BezierPoint.first)
			{
				vctBasePoint.push_back(m_currentPosition);
				vctBasePoint.push_back(m_BezierPoint.second);
				m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 3].data()) * m_scaleX;
				m_currentPosition.Y = bezierStart.Y -atof(VctPoint[n - 2].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data()) * m_scaleX;
				m_currentPosition.Y = bezierStart.Y-atof(VctPoint[n].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);
			}
			else
			{
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 3].data()) * m_scaleX;
				m_currentPosition.Y = bezierStart.Y -atof(VctPoint[n - 2].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data()) * m_scaleX;
				m_currentPosition.Y = bezierStart.Y -atof(VctPoint[n].data()) * m_scaleY;
				vctBasePoint.push_back(m_currentPosition);

			}
			for (int tm = 1; tm <= 50; tm++)
			{
				ClipperLibXYZ::DoublePoint FT_Temp;
				float t = tm / 50.0;
				FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 3) + vctBasePoint[1].X * t * pow(1 - t, 2) * 3 + vctBasePoint[2].X * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].X * pow(t, 3);
				FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 3) + vctBasePoint[1].Y * t * pow(1 - t, 2) * 3 + vctBasePoint[2].Y * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].Y * pow(t, 3);
				vctDoublePoint.push_back(FT_Temp);
			}
			savePenultPoint(vctBasePoint[2], vctBasePoint[3]);
			vctBasePoint.clear();
		}
		m_currentPosition = vctDoublePoint[vctDoublePoint.size() - 1];
	}
}

void svg::pausePathAa(string stringPoint, vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint)
{
	savePenultPoint();
}

ClipperLibXYZ::Path svg::DoublePoint2IntPoint(vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint)
{
	ClipperLibXYZ::Path polygon;
	for (int i = 0; i < vctDoublePoint.size(); i++)
	{
		size_t nSize = polygon.size();
		polygon.emplace_back();

		//m_currentMatrix3x3 矩阵偏移
		ClipperLibXYZ::DoublePoint dPoint;
		dPoint.X = m_currentMatrix3x3[0][0] * vctDoublePoint[i].X + m_currentMatrix3x3[0][1] * vctDoublePoint[i].Y + m_currentMatrix3x3[0][2];
		dPoint.Y = m_currentMatrix3x3[1][0] * vctDoublePoint[i].X + m_currentMatrix3x3[1][1] * vctDoublePoint[i].Y + m_currentMatrix3x3[1][2];


		polygon[nSize].X = SCALE1000(dPoint.X);
		polygon[nSize].Y = SCALE1000(dPoint.Y);
	}
	return polygon;
}

ClipperLibXYZ::Path svg::DoublePoint2IntPointEX(vector<ClipperLibXYZ::DoublePoint>& vctDoublePoint)
{
	ClipperLibXYZ::Path polygon;
	for (int i = 0; i < vctDoublePoint.size(); i++)
	{
		size_t nSize = polygon.size();
		polygon.emplace_back();

		//m_currentMatrix3x3 矩阵偏移
		ClipperLibXYZ::DoublePoint dPoint;
		dPoint.X = m_currentMatrix3x3[0][0] * vctDoublePoint[i].X + m_currentMatrix3x3[0][1] * vctDoublePoint[i].Y + m_currentMatrix3x3[0][2];
		dPoint.Y = -m_currentMatrix3x3[1][0] * vctDoublePoint[i].X - m_currentMatrix3x3[1][1] * vctDoublePoint[i].Y - m_currentMatrix3x3[1][2];


		polygon[nSize].X = SCALE1000(dPoint.X);
		polygon[nSize].Y = SCALE1000(dPoint.Y);
	}
	return polygon;
}

bool svg::isSpechars(char chars)
{
	char ch = toupper(chars);
	if (ch == 'M' || ch == 'L' || ch == 'H' || ch == 'V' || ch == 'S' || ch == 'Q' || ch == 'T' || ch == 'A' || ch == 'Z' || ch == 'C')
	{
		return true;
	} 
	else
	{
		return false;
	}
}

void svg::savePenultPoint(ClipperLibXYZ::DoublePoint penultPoint, ClipperLibXYZ::DoublePoint endPoint, bool isBezier)
{
	m_BezierPoint.first = isBezier;
	m_BezierPoint.second.X = endPoint.X * 2 - penultPoint.X;
	m_BezierPoint.second.Y = endPoint.Y * 2 - penultPoint.Y;
}

void svg::savePenultPoint(bool isBezier)
{
	m_BezierPoint.first = isBezier;
}

void svg::SplitString(const std::string& Src, std::vector<std::string>& Vctdest, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = Src.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		if (pos2 != 0)
		{
			Vctdest.push_back(Src.substr(pos1, pos2 - pos1));
		}
		pos1 = pos2 + c.size();
		pos2 = Src.find(c, pos1);
	}
	if (pos1 != Src.length())
	{
		Vctdest.push_back(Src.substr(pos1));
	}

}



void svg::SplitStringS(const std::string& Src, std::vector<std::string>& Vctdest, std::vector<std::string>& VctS)
{
	std::string::size_type pos1, pos2, pos2Mini = std::string::npos, pos3Mini = std::string::npos;
	for (std::string strTemp : VctS)
	{
		pos2 = Src.find(strTemp);
		if (std::string::npos == pos2)
		{
			continue;
		}
		pos2Mini = min(pos2Mini, pos2);
	}
	pos1 = 0;
	while (std::string::npos != pos2Mini)
	{
		if (pos2Mini != 0 && (pos2Mini - pos1 > 0))
		{
			Vctdest.push_back(Src.substr(pos1, pos2Mini - pos1));
		}
		pos1 = pos2Mini + 1;
		for (std::string strTemp : VctS)
		{
			pos2 = Src.find(strTemp, pos1);
			if (std::string::npos == pos2)
			{
				continue;
			}
			pos3Mini = min(pos3Mini, pos2);
		}
		pos2Mini = pos3Mini;
		pos3Mini = std::string::npos;
	}
	if (pos1 != Src.length())
	{
		Vctdest.push_back(Src.substr(pos1));
	}
}

ClipperLibXYZ::Paths* svg::outputPaths()
{
	return m_paths;
}