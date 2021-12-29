#include "fontlibrary.h"

namespace fmesh
{
	FontLibrary::FontLibrary()
	{
		FT_Error error = FT_Init_FreeType(&library);
		if (error) library = nullptr;
	}

	FontLibrary::~FontLibrary()
	{
		for (std::unordered_map<std::string, Font*>::iterator it = m_fonts.begin();
			it != m_fonts.end(); ++it)
			delete it->second;
		if (library) FT_Done_FreeType(library);
	}

	Font* FontLibrary::font(const char* name)
	{
		std::unordered_map<std::string, Font*>::iterator it = m_fonts.find(name);
		if (it != m_fonts.end())
			return it->second;

		Font* f = load(name);
		if (f) m_fonts.insert(std::unordered_map<std::string, Font*>::value_type(name, f));
		return f;
	}

	Font* FontLibrary::load(const char* name)
	{
		FT_Face face;
		FT_Error error = FT_New_Face(library, name, 0, &face);

		Font* f = error ? nullptr : new Font(face);
		return f;
	}
}