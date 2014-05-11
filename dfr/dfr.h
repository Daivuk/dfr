#pragma once

#include <string>

namespace dfr {
	struct sColor {
		unsigned char r, g, b;
	};

	enum eAlignV {
		ALIGNV_TOP		= 0x1,
		ALIGNV_CENTER	= 0x2,
		ALIGNV_BOTTOM	= 0x4,
	};

	enum eAlignH {
		ALIGNH_LEFT		= 0x8,
		ALIGNH_CENTER	= 0x10,
		ALIGNH_RIGHT	= 0x20,
		ALIGNH_JUSTIFY	= 0x40
	};

	enum eAlign {
		ALIGN_TOP_LEFT				= ALIGNV_TOP | ALIGNH_LEFT,
		ALIGN_TOP					= ALIGNV_TOP | ALIGNH_CENTER,
		ALIGN_TOP_RIGHT				= ALIGNV_TOP | ALIGNH_RIGHT,
		ALIGN_LEFT					= ALIGNV_CENTER | ALIGNH_LEFT,
		ALIGN_CENTER				= ALIGNV_CENTER | ALIGNH_CENTER,
		ALIGN_RIGHT					= ALIGNV_CENTER | ALIGNH_RIGHT,
		ALIGN_BOTTOM_LEFT			= ALIGNV_BOTTOM | ALIGNH_LEFT,
		ALIGN_BOTTOM				= ALIGNV_BOTTOM | ALIGNH_CENTER,
		ALIGN_BOTTOM_RIGHT			= ALIGNV_BOTTOM | ALIGNH_RIGHT,
		ALIGN_TOP_LEFT_JUSTIFY		= ALIGNV_TOP | ALIGNH_LEFT | ALIGNH_JUSTIFY,
		ALIGN_TOP_JUSTIFY			= ALIGNV_TOP | ALIGNH_CENTER | ALIGNH_JUSTIFY,
		ALIGN_TOP_RIGHT_JUSTIFY		= ALIGNV_TOP | ALIGNH_RIGHT | ALIGNH_JUSTIFY,
		ALIGN_LEFT_JUSTIFY			= ALIGNV_CENTER | ALIGNH_LEFT | ALIGNH_JUSTIFY,
		ALIGN_CENTER_JUSTIFY		= ALIGNV_CENTER | ALIGNH_CENTER | ALIGNH_JUSTIFY,
		ALIGN_RIGHT_JUSTIFY			= ALIGNV_CENTER | ALIGNH_RIGHT | ALIGNH_JUSTIFY,
		ALIGN_BOTTOM_LEFT_JUSTIFY	= ALIGNV_BOTTOM | ALIGNH_LEFT | ALIGNH_JUSTIFY,
		ALIGN_BOTTOM_JUSTIFY		= ALIGNV_BOTTOM | ALIGNH_CENTER | ALIGNH_JUSTIFY,
		ALIGN_BOTTOM_RIGHT_JUSTIFY	= ALIGNV_BOTTOM | ALIGNH_RIGHT | ALIGNH_JUSTIFY
	};

	void init();

	/**
		Draw text using specific font and point size onto an image buffer.

		@param in_text Text to render. i.e: "Hello World!"

		@param out_buffer Your image buffer. This should be of size in_width * in_height * 4

		@param in_width Width of your image

		@param in_height Height of your image

		@param in_font Font filename. i.e: "Content/Fonts/OpenSans-Semibold.ttf"

		@param in_size Point size for the font.

		@param in_wordWrap Enable multi lines if the text is too long

		@param in_color RGB color for the text. Values are in range 0-255

		@param in_align One of dfr::eAlign value.

		@param in_minSize Minimum size allowed for the font. If the text doesn't fit in the image,
		the renderer will pick a point size bellow and try again. Until this value is reached.
		If a value of 0 is passed, autosize is disabled.

		@param out_containingRect Array of 4 ints. Will be filled the containing rectangle of the text
		rendered. x,y,w,h
	*/
	void drawText(
		const std::string& in_text,
		unsigned char* out_buffer, const unsigned int in_width, const unsigned int in_height,
		const std::string& in_font, const unsigned int in_size,
		bool in_wordWrap = true,
		const sColor& in_color = { 255, 255, 255 },
		eAlign in_align = ALIGN_TOP_LEFT,
		const unsigned int in_minSize = 0,
		int* out_containingRect = NULL,
		bool in_rightToLeft = false);
	void drawText(
		const std::wstring& in_text,
		unsigned char* out_buffer, const unsigned int in_width, const unsigned int in_height,
		const std::string& in_font, const unsigned int in_size,
		bool in_wordWrap = true,
		const sColor& in_color = { 255, 255, 255 },
		eAlign in_align = ALIGN_TOP_LEFT,
		const unsigned int in_minSize = 0,
		int* out_containingRect = NULL,
		bool in_rightToLeft = false);
};
