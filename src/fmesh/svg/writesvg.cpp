#include "writesvg.h"
#include "tinyxml/tinyxml.h"

#include <string>
#include "fmesh/generate/earpolygon.h"
#include "fmesh/generate/polyfiller.h"
#include "mmesh/clipper/circurlar.h"
#include "fmesh/contour/polytree.h"

namespace cdrdxf
{
	void writesvg(ClipperLib::PolyTree* tree, const std::string& file)
	{
		if (tree == nullptr)
			return;

		std::vector<ClipperLib::Path*> paths;
		auto func = [&paths](ClipperLib::PolyNode* node) {
			int depth = fmesh::testPolyNodeDepth(node);
			if (depth == 2 || depth == 3)
				paths.push_back(&node->Contour);
		};
		mmesh::loopPolyTree(func, tree);

		std::string temp, tempstring;
		std::string tempstr;
		int height = 1;
		int width = 1;

		char buf[512];
		std::sprintf(buf, "<?xml version=\"1.0\"  standalone='no' > "
			"<!-- IBoard SVG File -->"
			"<svg height=\"%d\" width=\"%d\">"
			"<!-- SVG File --> "
			"</svg>", height, width);
		temp =buf;

		const char* demoStart = temp.data();
		if (file != "")
			tempstring.append(file);
		else
			tempstring.append("iboardfile.svg");

		const   char* FileName = tempstring.data();
		TiXmlDocument doc(FileName);
		doc.Parse(demoStart);

		if (doc.Error())
		{
			return;
		}
		else
			doc.SaveFile();

		TiXmlDocument doc1(file.c_str());
		bool loadOkay = doc1.LoadFile();
		if (!loadOkay)
		{
			return ;
		}
		TiXmlNode* node = doc1.FirstChild("svg");
		assert(node);
		// path:
		for (size_t i = 0; i < paths.size(); i++)
		{
			if (!paths.at(i)->size())
				continue;
			TiXmlElement child("polygon");
			std::string str;
			for (size_t j = 0; j < paths.at(i)->size(); j++)
			{
				str+=std::to_string(paths.at(i)->at(j).X);
				str += ",";
				str += std::to_string(paths.at(i)->at(j).Y);
				str += " ";
			}
			child.SetAttribute("points", str.c_str());
			child.SetAttribute("style", "fill-opacity:0;stroke:red");
			node->InsertEndChild(child);
		}
		doc1.SaveFile();
	}

	FMESH_API void writesvg(std::vector<ClipperLib::PolyTree*>& trees, const std::string& file)
	{
		if (!trees.size())
			return;

		std::vector<ClipperLib::Path*> paths;
		std::vector<std::vector<ClipperLib::Path*>> vecPaths;

		auto func = [&paths](ClipperLib::PolyNode* node) {
			int depth = fmesh::testPolyNodeDepth(node);
			if (depth == 2 || depth == 3)
				paths.push_back(&node->Contour);
		};
		for (size_t i=0;i< trees.size();i++)
		{
			paths.clear();
			mmesh::loopPolyTree(func, trees.at(i));
			vecPaths.push_back(paths);
		}

		std::string temp, tempstring;
		std::string tempstr;
		int height = 1;
		int width = 1;

		char buf[512];
		std::sprintf(buf, "<?xml version=\"1.0\"  standalone='no' > "
			"<!-- IBoard SVG File -->"
			"<svg height=\"%d\" width=\"%d\">"
			"<!-- SVG File --> "
			"</svg>", height, width);
		temp = buf;

		const char* demoStart = temp.data();
		if (file != "")
			tempstring.append(file);
		else
			tempstring.append("iboardfile.svg");

		const   char* FileName = tempstring.data();
		TiXmlDocument doc(FileName);
		doc.Parse(demoStart);

		if (doc.Error())
		{
			return;
		}
		else
			doc.SaveFile();

		TiXmlDocument doc1(file.c_str());
		bool loadOkay = doc1.LoadFile();
		if (!loadOkay)
		{
			return;
		}
		TiXmlNode* node = doc1.FirstChild("svg");
		assert(node);
		// path:

		for (size_t num=0;num< vecPaths.size();num++)
		{
			
			TiXmlElement child("path");
			std::string str="";
			for (size_t i = 0; i < vecPaths.at(num).size(); i++)
			{
				if (!vecPaths.at(num).at(i)->size()>1)
					continue;
				str += "M";//moveto
				str += std::to_string(vecPaths.at(num).at(i)->at(0).X/1000.f);
				str += " ";
				str += std::to_string(-vecPaths.at(num).at(i)->at(0).Y/1000.f);
				str += "L";
				//lineto
				for (size_t j = 1; j < vecPaths.at(num).at(i)->size(); j++)
				{
					
					str += std::to_string(vecPaths.at(num).at(i)->at(j).X/1000.f);
					str += ",";
					str += std::to_string(-vecPaths.at(num).at(i)->at(j).Y/1000.f);
					str += " ";
				}
				str += "z";//end
			}
			child.SetAttribute("d", str.c_str());
			child.SetAttribute("style", "fill-opacity:0;stroke:red");
			node->InsertEndChild(child);
		}
		doc1.SaveFile();
	}

}