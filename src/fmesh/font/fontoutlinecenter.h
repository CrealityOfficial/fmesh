#ifndef _FONTOUTLINECENTER_1603209926789_H
#define _FONTOUTLINECENTER_1603209926789_H
#include <clipper/clipper.hpp>
#include "fmesh/common/export.h"

namespace fmesh
{
	class FontLibrary;
	class Font;

	class FMESH_API FontOutlineCenter
	{
	public:
		FontOutlineCenter();
		virtual ~FontOutlineCenter();

		void load(const std::string& name);
		ClipperLib::PolyTree* get(int charCode);
		ClipperLib::Paths* getPath(int charCode, double expectLen = 50.0);
		ClipperLib::Paths* getPath(const std::string& font, int charCode, double expectLen = 50);

		void addSearchPath(const std::string& dir);
	protected:
		fmesh::FontLibrary* library;
		fmesh::Font* font;

		std::vector<std::string> m_searchDirs;
	};
}
#endif // _FONTOUTLINECENTER_1603209926789_H