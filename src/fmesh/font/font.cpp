#include "font.h"
#include "outline.h"

#include <iostream>
#define SIMPLERATIO 16
namespace fmesh
{
	Font::Font(FT_Face _face)
		: face(_face)
	{
	}

	Font::~Font()
	{

	}

	Outline* Font::get(FT_ULong charCode)
	{
		std::unordered_map<FT_ULong, Outline*>::iterator it = m_outlines.find(charCode);
		if (it != m_outlines.end())
			return it->second;

		Outline* ol = load(charCode);
		if (ol)
		{
			ol->end();
			m_outlines.insert(std::unordered_map<FT_ULong, Outline*>::value_type(charCode, ol));
		}
		return ol;
	}

	struct LData
	{
		Outline* oline;
		bool firstpass;
		bool startcontour;
		double xMin;
		double factor;
		int yMax;

		double prevx;
		double prevy;
	};

	int moveTo(FT_Vector* to, void* fp)
	{
		LData* data = (LData*)fp;

		if (data->firstpass)
		{
			if (to->x < data->xMin)
				data->xMin = to->x;
		}
		else
		{
			data->prevx = to->x;
			data->prevy = to->y;

			if (data->startcontour)
			{
				data->startcontour = false;
				data->oline->push((double)(to->x - data->xMin) * data->factor, (double)to->y * data->factor);
			}
			else
			{
				data->oline->end();
				data->oline->push((double)(to->x - data->xMin) * data->factor, (double)to->y * data->factor);
			}
		}
		return 0;
	}

	int lineTo(FT_Vector* to, void* fp)
	{
		LData* data = (LData*)fp;

		if (data->firstpass)
		{
			if (to->x < data->xMin)
				data->xMin = to->x;
		}
		else
		{
			if (data->startcontour)
			{
				data->oline->push((double)(to->x - data->xMin) * data->factor, (double)to->y * data->factor);
				data->startcontour = false;
			}
			else
			{
				data->oline->push((double)(to->x - data->xMin) * data->factor, (double)to->y * data->factor);
			}

			data->prevx = to->x;
			data->prevy = to->y;

			if (to->y > data->yMax)
			{
				data->yMax = to->y;
			}
		}
		return 0;
	}

	int conicTo(FT_Vector* control, FT_Vector* to, void* fp)
	{
		LData* data = (LData*)fp;

		if (data->firstpass) {
			if (to->x < data->xMin)
				data->xMin = to->x;
			return 0;
		}

		double px, py;
		double ox = data->prevx;
		double oy = data->prevy;

		if (data->startcontour)
		{
			data->oline->push((ox - data->xMin) * data->factor, oy * data->factor);
			data->startcontour = false;
		}
		else
		{
			data->oline->push((ox - data->xMin) * data->factor, oy * data->factor);
		}
		for (double t = 0.0; t <= 1.0; t += 1.0 / SIMPLERATIO)
		{
			px = pow(1.0 - t, 2) * data->prevx + 2 * t * (1.0 - t) * control->x + t * t * to->x;
			py = pow(1.0 - t, 2) * data->prevy + 2 * t * (1.0 - t) * control->y + t * t * to->y;
			data->oline->push((double)(px - data->xMin) * data->factor, (double)py * data->factor);

			//			ox = px;
			//			oy = py;
		}

		data->prevx = to->x;
		data->prevy = to->y;

		if (to->y > data->yMax)
			data->yMax = to->y;
		return 0;
	}

	int cubicTo(FT_Vector* /*control1*/, FT_Vector* /*control2*/, FT_Vector* to, void* fp)
	{
		LData* data = (LData*)fp;

		if (data->firstpass)
		{
			if (to->x < data->xMin)
				data->xMin = to->x;
		}
		else
		{
			if (data->startcontour)
			{
				data->oline->push((double)(to->x - data->xMin) * data->factor, (double)to->y * data->factor);
				data->startcontour = false;
			}
			else
			{
				data->oline->push((double)(to->x - data->xMin) * data->factor, (double)to->y * data->factor);
			}

			data->prevx = to->x;
			data->prevy = to->y;

			if (to->y > data->yMax)
			{
				data->yMax = to->y;
			}
		}
		return 0;
	}

	static const FT_Outline_Funcs funcs
		= {
			  (FT_Outline_MoveTo_Func)moveTo,
			  (FT_Outline_LineTo_Func)lineTo,
			  (FT_Outline_ConicTo_Func)conicTo,
			  (FT_Outline_CubicTo_Func)cubicTo,
			  0, 0
	};

	Outline* Font::load(FT_ULong charCode)
	{
		FT_Glyph glyph;

		// load glyph
		FT_Error error = FT_Load_Glyph(face,
			FT_Get_Char_Index(face, charCode),
			FT_LOAD_NO_BITMAP | FT_LOAD_NO_SCALE);
		if (error)
		{
			return nullptr;
		}

		FT_Get_Glyph(face->glyph, &glyph);
		FT_OutlineGlyph og = (FT_OutlineGlyph)glyph;
		if (face->glyph->format != ft_glyph_format_outline)
		{
			std::cerr << "Not an outline font\n";
			return nullptr;
		}

		Outline* oline = new Outline();

		LData data;
		data.oline = oline;
		data.firstpass = true;
		data.startcontour = false;
		data.xMin = 1000.0;
		data.yMax = 922;
		data.factor = 1.0 / (1.0 / 9.0 * data.yMax);;

		error = FT_Outline_Decompose(&(og->outline), &funcs, &data);
		if (!error)
		{
			data.firstpass = false;
			data.startcontour = true;
			error = FT_Outline_Decompose(&(og->outline), &funcs, &data);
			if (error)
			{
				delete oline;
				oline = nullptr;
			}
		}
		else
		{
			delete oline;
			oline = nullptr;
		}

		return oline;
	}
}