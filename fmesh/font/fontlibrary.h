#ifndef FMESH_FONTLIBRARY_1603205674446_H
#define FMESH_FONTLIBRARY_1603205674446_H
#include "fmesh/font/font.h"

#include <unordered_map>
#include <string>

namespace fmesh
{
	class FontLibrary
	{
	public:
		FontLibrary();
		~FontLibrary();

		Font* font(const char* name);
	protected:
		Font* load(const char* name);
	protected:
		FT_Library library;
		std::unordered_map<std::string, Font*> m_fonts;
	};
}

#endif // FMESH_FONTLIBRARY_1603205674446_H