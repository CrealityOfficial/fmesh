#include "fontoutlinecenter.h"
#include "fmesh/font/fontlibrary.h"
#include "fmesh/font/outline.h"
#include "fmesh/contour/contour.h"
#include "fmesh/contour/path.h"

#include <iostream>
namespace fmesh
{

	FontOutlineCenter::FontOutlineCenter()
		: library(nullptr)
		, font(nullptr)
	{
		library = new FontLibrary();
	}

	FontOutlineCenter::~FontOutlineCenter()
	{

	}

	void FontOutlineCenter::load(const std::string& name)
	{
		Font* f = nullptr;
		for (const std::string& dir : m_searchDirs)
		{
			std::string fontFile = dir + std::string("/resources/fonts/") + name + std::string(".ttf");
			f = library->font(fontFile.c_str());

			if (!f)
			{
				fontFile = dir + std::string("/fonts/") + name;
				f = library->font(fontFile.c_str());
			}

			if (f)
				break;
		}

		if (!f)
		{
			std::cout << "load font " << name << " error . try load without .ttf" << std::endl;
		}

		if (f) font = f;
	}

	void FontOutlineCenter::addSearchPath(const std::string& dir)
	{
		m_searchDirs.push_back(dir);
	}

	ClipperLib::PolyTree* FontOutlineCenter::get(int charCode)
	{
		if (!font) return nullptr;
		Outline* outline = font->get((FT_ULong)charCode);
		return fmesh::convertOutline2PolyTree(outline);
	}

	ClipperLib::Paths* FontOutlineCenter::getPath(int charCode, double expectLen)
	{
		if (!font) return nullptr;
		Outline* outline = font->get((FT_ULong)charCode);

		ClipperLib::Paths* paths = new ClipperLib::Paths();
		*paths = outline->pathes();
		scalePath2ExpectLen(paths, expectLen);

		return paths;
	}

	ClipperLib::Paths* FontOutlineCenter::getPath(const std::string& font, int charCode, double expectLen)
	{
		load(font);
		return getPath(charCode, expectLen);
	}
}