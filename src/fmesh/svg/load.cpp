#include "load.h"
#include "tinyxml/tinyxml.h"
#include "svg.h"

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
}