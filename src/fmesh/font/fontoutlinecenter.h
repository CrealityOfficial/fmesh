#ifndef _FONTOUTLINECENTER_1603209926789_H
#define _FONTOUTLINECENTER_1603209926789_H
#include <clipper/clipper.hpp>

namespace fmesh
{
	class FontLibrary;
	class Font;
	class Outline;

	class FontOutlineCenter
	{
	public:
		FontOutlineCenter();
		virtual ~FontOutlineCenter();

		void load(const std::string& name);
		ClipperLib::PolyTree* get(int charCode);
		ClipperLib::Paths* getPath(int charCode);

		void addSearchPath(const std::string& dir);
	protected:
		fmesh::FontLibrary* library;
		fmesh::Font* font;

		std::vector<std::string> m_searchDirs;
	};
}
#endif // _FONTOUTLINECENTER_1603209926789_H