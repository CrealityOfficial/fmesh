#include "load.h"
#include "tinyxml/tinyxml.h"
#include "svg.h"
#include "multisvg.h"

namespace cdrdxf
{
	ClipperLib::Paths* loadSVGFile(const char* fileName)
	{
		TiXmlDocument doc;
		if (!doc.LoadFile(fileName))
		{
			return nullptr;
		}
		TiXmlElement* root = doc.FirstChildElement();
		if (root == NULL)
		{
			doc.Clear();
			return nullptr;
		}

		svg mySVG;
		mySVG.pauseSVGEX(root);
		return mySVG.outputPaths();
	}

	template <class T>
	void parseSVG(TiXmlElement* root , T* t)
	{
		TiXmlElement* elemBrother = root;
		while (elemBrother)
		{
			string strElemValue = elemBrother->Value();
			if (strElemValue == SVG_LINE)
			{
				float start_x = atof(elemBrother->Attribute("x1"));
				float start_y = atof(elemBrother->Attribute("y1"));
				float end_x = atof(elemBrother->Attribute("x2"));
				float end_y = atof(elemBrother->Attribute("y2"));
				//处理线段
				t->addLine(start_x, start_y, end_x, end_y);
			}
			else if (strElemValue == SVG_POLYLINE)
			{
				string strPoints = elemBrother->Attribute("points");
				//处理多线段
				t->addPolyLine(strPoints);
			}
			else if (strElemValue == SVG_RECT)
			{
				float width = atof(elemBrother->Attribute("width"));
				float height = atof(elemBrother->Attribute("height"));
				float leftTopX = atof(elemBrother->Attribute("x"));
				float leftTopY = atof(elemBrother->Attribute("y"));
				//处理矩形
				t->addRect(width, height, leftTopX, leftTopY);
			}
			else if (strElemValue == SVG_CIRCLE)
			{
				float cx = atof(elemBrother->Attribute("cx"));
				float cy = atof(elemBrother->Attribute("cy"));
				float rr = atof(elemBrother->Attribute("r"));
				//处理圆
				t->addEllipse(cx, cy, rr, rr);
			}
			else if (strElemValue == SVG_ELLIPSE)
			{
				float cx = atof(elemBrother->Attribute("cx"));
				float cy = atof(elemBrother->Attribute("cy"));
				float rx = atof(elemBrother->Attribute("rx"));
				float ry = atof(elemBrother->Attribute("ry"));
				//处理椭圆
				t->addEllipse(cx, cy, rx, ry);
			}
			else if (strElemValue == SVG_POLYGON)
			{
				string strPoints = elemBrother->Attribute("points");
				//处理多边形
				t->addPolygonS(strPoints);
			}
			else if (strElemValue == SVG_PATH)
			{
				string strPath = elemBrother->Attribute("d");
				//处理path文件
				t->addPathS(strPath);
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
						parseSVG(root->FirstChildElement(), t);//获取当前兄弟节点的子节点
					}
				}
			}
		}
	}

	template <class T>
	void parseSVGFile(const char* fileName, T* t)
	{
		TiXmlDocument doc;
		if (!doc.LoadFile(fileName))
		{
			return;
		}
		TiXmlElement* root = doc.FirstChildElement();
		if (root == NULL)
		{
			doc.Clear();
			return;
		}

		parseSVG(root, t);
	}

	std::vector<ClipperLib::Paths*> loadMultiSVGFile(const char* fileName)
	{
		fmesh::MultiSVG svg;
		parseSVGFile(fileName, &svg);
		return svg.take();
	}
}