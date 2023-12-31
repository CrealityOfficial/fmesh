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
		m_searchDirs.push_back("");
	}

	FontOutlineCenter::~FontOutlineCenter()
	{
		delete library;
	}

	bool FontOutlineCenter::load(const std::string& name)
	{
		Font* f = nullptr;
		for (const std::string& dir : m_searchDirs)
		{
			std::string fontFile = dir + std::string("/fonts/") + name + std::string(".ttf");
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

		return f != nullptr;
	}

	bool FontOutlineCenter::loadAbs(const std::string& name)
	{
		Font* f = library->font(name.c_str());
		if (!f)
		{
			std::cout << "load font " << name << " error . try load without .ttf" << std::endl;
		}

		if (f) font = f;

		return f != nullptr;
	}

	void FontOutlineCenter::setCurrent(const std::string& name)
	{
		Font* f = library->font(name.c_str());
		if (f) font = f;
	}

	void FontOutlineCenter::addSearchPath(const std::string& dir)
	{
		m_searchDirs.push_back(dir);

		std::string resourceDir = dir + std::string("/resources");
		m_searchDirs.push_back(resourceDir);
	}

	ClipperLibXYZ::PolyTree* FontOutlineCenter::get(int charCode)
	{
		if (!font) return nullptr;
		Outline* outline = font->get((FT_ULong)charCode);
		return fmesh::convertOutline2PolyTree(outline);
	}

	ClipperLibXYZ::Paths* FontOutlineCenter::getPath(int charCode, double expectLen, const int simpleRatio)
	{
		if (!font) return nullptr;
		Outline* outline = font->get((FT_ULong)charCode, simpleRatio);

		ClipperLibXYZ::Paths* paths = new ClipperLibXYZ::Paths();
		*paths = outline->pathes();
		scalePath2ExpectLen(paths, expectLen);

		return paths;
	}

	ClipperLibXYZ::Paths* FontOutlineCenter::getPath(const std::string& fontFile, int charCode, double expectLen)
	{
		load(fontFile);
		return getPath(charCode, expectLen);
	}
}