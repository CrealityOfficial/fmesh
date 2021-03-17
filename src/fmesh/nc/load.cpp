#include "load.h"
#include <stdio.h>
#include <ctype.h>
#include <map>

#include <memory>
#define LINECOUNT 100
namespace cdrdxf
{
	ClipperLib::Paths* loadNCFile(const char* fileName)
	{
 		ClipperLib::Paths* paths = nullptr;
 		return paths;
	}

	ClipperLib::Paths* loadSplineFromNCFile(const char* fileName)
	{
		ClipperLib::Paths* paths = nullptr;
		return paths;
	}

	int divisionXYZ(std::string str, ClipperLib::IntPoint& point)
	{
		int result = 0;
		if (str.empty())
			return result;
		str = str.substr(0, str.size() - 1);
		//std::map<std::string, std::string> _vct;
		for (int i = str.size() - 1; i >= 0; i--) {
			char c = str.at(i);
			if ('A' <= c && c <= 'Z' || 'a' <= c && c <= 'z')
			{
				//_vct.insert(make_pair(str.substr(i,1),str.substr(i+1, str.size())));
				if ('X' == c || 'x' == c)
				{
					point.X = (ClipperLib::cInt)(1000.0 * atof(str.substr(i + 1, str.size()).c_str()));
					result = 1;
				}
				else if ('Y' == c || 'y' == c)
				{
					point.Y = (ClipperLib::cInt)(1000.0 * atof(str.substr(i + 1, str.size()).c_str()));
					result = 1;
				}
				else if ('Z' == c || 'z' == c)
				{
					point.Z = (ClipperLib::cInt)(1000.0 * atof(str.substr(i + 1, str.size()).c_str()));
					result = 2;
				}
				str = str.substr(0, i);
			}
		}

		return result;
	}

	std::vector<ClipperLib::Paths*> loadMultiNCFile(const char* fileName)
	{
		std::vector<ClipperLib::Paths*> tmp;
		char str[LINECOUNT + 1];
		FILE* fp;
		if ((fp = fopen(fileName, "r")) == NULL) {
			return tmp;
		}
		
		//read per line
		ClipperLib::IntPoint _point(0, 0, 0);
		ClipperLib::Paths* paths = new ClipperLib::Paths();
		ClipperLib::Path path;
		while (fgets(str, LINECOUNT, fp) != NULL) {					
			int type = divisionXYZ(str, _point);	
			if (type == 1)
			{
				path.push_back(_point);
			} 
			else if (type == 2)
			{
				if (path.size()>2)
				{
					paths->push_back(path);
					tmp.push_back(paths);
					paths = new ClipperLib::Paths();
					path.push_back(_point);
				}
				path.clear();
			}
		}

		fclose(fp);
		return tmp;
	}
}