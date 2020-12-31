#include "writesvg.h"
#include "tinyxml/tinyxml.h"

namespace cdrdxf
{
	void writesvg(ClipperLib::PolyTree* tree, const std::string& file)
	{
		std::string temp, tempstring;
		std::string tempstr;
		//生成SVG文件框
		int height = 1;
		int width = 1;

		char buf[1024];
		std::sprintf(buf, "<?xml version=\"1.0\"  standalone='no' > "
			"<!-- IBoard SVG File -->"
			"<svg height=\"%d\" width=\"%d\">"
			"<!-- SVG File --> "
			"</svg>", height, width);
		tempstr=buf;

		const char* demoStart = temp.data();
		//若文件名不为空
		if (file != "")
			tempstring.append(file);
		else
			tempstring.append("iboardfile.svg");

		const   char* FileName = tempstring.data();
		TiXmlDocument doc(FileName);
		doc.Parse(demoStart);

		if (doc.Error())
		{
			//AfxMessageBox("some error ,fix here");
			return;
		}
		else
			doc.SaveFile();
		//


		/////使用tinyxml操作SVG文件/////
		TiXmlDocument doc1("iboardfile.svg");
		bool loadOkay = doc.LoadFile();
		if (!loadOkay)
		{
			return ;
		}
		TiXmlNode* node = doc.FirstChild("svg");
		assert(node);
		//向SVG文件插入一个圆
		TiXmlElement child("ellipse");
		child.SetAttribute("cx", "50");
		child.SetAttribute("cy", "50");
		child.SetAttribute("rx", "100");
		child.SetAttribute("ry", "100");
		child.SetAttribute("style", "fill:Red;stroke:red");
		node->InsertEndChild(child);
		doc1.SaveFile();
		/////插入圆成功，保存文件
	}
}