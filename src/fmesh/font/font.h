#ifndef FMESH_FONT_1603205674463_H
#define FMESH_FONT_1603205674463_H
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include FT_OUTLINE_H
#include FT_GLYPH_H

#include <unordered_map>

namespace fmesh
{
	class Outline;
	class Font
	{
	public:
		Font(FT_Face _face);
		~Font();

		Outline* get(FT_ULong charCode, int simpleRatio =12);
	protected:
		Outline* load(FT_ULong charCode, int simpleRatio = 12);
	protected:
		FT_Face face;
		std::unordered_map<FT_ULong, Outline*> m_outlines;
	};
}

#endif // FMESH_FONT_1603205674463_H