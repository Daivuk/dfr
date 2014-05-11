#include "dfr.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <assert.h>
#include <map>
#include <vector>
#include <locale>
#include <codecvt>
#include <mutex>

namespace dfr {

	std::mutex g_ttfFacesMutex;
	FT_Library g_ttfLibrary;
	std::map<std::string, FT_Face> g_faces;
	bool g_isInitialized = false;

	void init() {
		if (g_isInitialized) return;
		assert(!FT_Init_FreeType(&g_ttfLibrary));
		g_isInitialized = true;
	}

	inline int ttf_max(int a, int b) {
		return a - ((a - b) & (a - b) >> 31);
	}

	inline int ttf_min(int a, int b) {
		return a + (((b - a) >> 31) & (b - a));
	}

	void drawText(
		const std::string& in_text,
		unsigned char* out_buffer, const unsigned int in_width, const unsigned int in_height,
		const std::string& in_font, const unsigned int in_size,
		bool in_wordWrap,
		const sColor& in_color,
		eAlign in_align,
		const unsigned int in_minSize) {

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wText = converter.from_bytes(in_text);

		drawText(wText, out_buffer, in_width, in_height, in_font, in_size, in_wordWrap, in_color, in_align, in_minSize);
	}

	void drawText(
		const std::wstring& in_text,
		unsigned char* out_buffer, const unsigned int in_width, const unsigned int in_height,
		const std::string& in_font, const unsigned int in_size,
		bool in_wordWrap,
		const sColor& in_color,
		eAlign in_align,
		const unsigned int in_minSize) {
		FT_Face face;

		g_ttfFacesMutex.lock();
		{
			const auto& it = g_faces.find(in_font);
			if (it == g_faces.end()) {
				assert(!FT_New_Face(
					g_ttfLibrary,
					in_font.c_str(),
					0,
					&face));
				g_faces[in_font] = face;
			}
			else {
				face = it->second;
			}
		}
		g_ttfFacesMutex.unlock();

		unsigned int pointSize = in_size;
		struct sLine {
			int width = 0;
			int from = 0;
			int to = 0;
		};
		while (true) {
			assert(!FT_Set_Pixel_Sizes(
				face,
				0,
				pointSize));

			FT_GlyphSlot	slot = face->glyph;
			int				pen_x, pen_y, n, x, y, k;
			int				num_chars = (int) in_text.size();
			wchar_t*			text = (wchar_t*) in_text.c_str();
			unsigned char	alpha;
			FT_Error		error;
			int lineHeight = face->size->metrics.height >> 6;
			pen_x = 0;
			pen_y = 0;
			int maxW = 0;
			int alignOffsetX = 0;
			int alignOffsetY = 0;
			int lastWordStart = -1;
			int lastWordWidth = 0;
			std::vector<sLine> lines;
			sLine currentLine;

			for (n = 0; n < num_chars; ++n) {
				// Get the glyph metrics only
				FT_UInt  glyph_index;
				glyph_index = FT_Get_Char_Index(face, text[n]);
				error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
				if (error) continue;

				int advance = slot->advance.x >> 6;
				if (n > 0) {
					FT_UInt leftGlyph = FT_Get_Char_Index(face, text[n - 1]);
					FT_Vector kerning;
					FT_Get_Kerning(face, leftGlyph, glyph_index, FT_KERNING_DEFAULT, &kerning);
					advance += kerning.x;
				}
				int gW = advance;

				if (text[n] == '\n') {
					// We will new line and ignore that return
					currentLine.width = pen_x;
					currentLine.to = n - 1;
					lines.push_back(currentLine);
					currentLine.from = currentLine.to + 2;
					lastWordWidth = 0;
					pen_x = 0;
					pen_y += lineHeight;
					continue;
				}

				if (pen_x + gW >= (int) in_width && in_wordWrap) {
					maxW = ttf_max(maxW, pen_x);

					if (text[n] == ' ') {
						// We will new line and ignore that space
						currentLine.width = pen_x;
						currentLine.to = n - 1;
						lines.push_back(currentLine);
						currentLine.from = currentLine.to + 1;
						lastWordWidth = 0;
						pen_x = 0;
						pen_y += lineHeight;
						continue;
					}

					// This character will go over, we will break line
					if (lastWordStart == -1) {
						// We have to cut this word in half, its bigger
						// than the row!
						currentLine.width = pen_x;
						currentLine.to = n - 1;
						lines.push_back(currentLine);
						currentLine.from = currentLine.to + 1;
						lastWordWidth = 0;
						pen_x = 0;
						pen_y += lineHeight;
						--n;
						continue;
					}
					else {
						currentLine.width = lastWordWidth;
						currentLine.to = lastWordStart - 1;
						lines.push_back(currentLine);
						currentLine.from = currentLine.to + 1;
						pen_x = 0;
						pen_y += lineHeight;
						n = lastWordStart - 1;
						lastWordWidth = 0;
						lastWordStart = -1;
						continue;
					}
				}

				if (text[n] == ' ') {
					lastWordWidth = pen_x;
					lastWordStart = n + 1;
				}

				pen_x += gW;
			}

			if (pen_x) {
				maxW = ttf_max(maxW, pen_x);
				currentLine.width = pen_x;
				currentLine.to = n - 1;
				lines.push_back(currentLine);
			}

			// Now render line by line
			pen_x = 0;

			// Align vertically
			if (in_align & ALIGNV_TOP)
				pen_y = face->ascender >> 6;
			else if (in_align & ALIGNV_CENTER)
				pen_y = ((int) in_height - (int) (lines.size() - 1) * lineHeight - (face->descender >> 6)) / 2;
			else if (in_align & ALIGNV_BOTTOM)
				pen_y = (int) in_height + (face->descender >> 6) - ((int) lines.size() - 1) * (lineHeight >> 6);

			// It is going to fit? Try a smaller fixe it not
			if (in_minSize && pointSize > in_minSize) {
				if (maxW > (int) in_width ||
					(lines.size() * lineHeight) > (int) in_height) {
					--pointSize;
					continue;
				}
			}

			size_t lineCpt = 0;
			for (const auto& line : lines) {
				if (in_align & ALIGNH_LEFT)
					pen_x = 0;
				else if (in_align & ALIGNH_CENTER)
					pen_x = ((int) in_width - line.width) / 2;
				else if (in_align & ALIGNH_RIGHT)
					pen_x = (int) in_width - line.width;
				bool allowJustify = lineCpt < lines.size() - 1 && in_align & ALIGNH_JUSTIFY;
				if (allowJustify) {
					if (lines[lineCpt + 1].to - lines[lineCpt + 1].from <= 0) allowJustify = false;
				}
				int leftOver = (int) in_width - line.width;
				int cCount = line.to - line.from + 1;
				if (allowJustify) pen_x = 0;
				for (n = line.from; n <= line.to; ++n) {
					// Get the glyph
					FT_UInt  glyph_index;
					glyph_index = FT_Get_Char_Index(face, text[n]);
					error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
					if (error) continue;
					error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
					if (error) continue;

					int glyphX = pen_x + (slot->metrics.horiBearingX >> 6);
					int glyphY = pen_y - (slot->metrics.horiBearingY >> 6);

					// Justify
					if (allowJustify) {
						int i = n - line.from;
						if (i > 0) {
							glyphX += leftOver * i / cCount;
						}
					}

					FT_Bitmap* bitmap = &slot->bitmap;

					int limits[4] = {
						ttf_max(0, glyphX) - glyphX,
						ttf_max(0, glyphY) - glyphY,
						ttf_min((int) in_width, bitmap->width + glyphX) - glyphX,
						ttf_min((int) in_height, bitmap->rows + glyphY) - glyphY,
					};
					for (y = limits[1]; y < limits[3]; y++) {
						for (x = limits[0]; x < limits[2]; x++) {
							k = ((y + glyphY) * in_width + x + glyphX) * 4;
							alpha = ttf_max(out_buffer[k + 3], bitmap->buffer[y * bitmap->pitch + x]);
							out_buffer[k + 0] = in_color.r * alpha / 255;
							out_buffer[k + 1] = in_color.g * alpha / 255;
							out_buffer[k + 2] = in_color.b * alpha / 255;
							out_buffer[k + 3] = alpha;
						}
					}

					int advance = slot->advance.x >> 6;
					if (n > line.from) {
						FT_UInt leftGlyph = FT_Get_Char_Index(face, text[n - 1]);
						FT_Vector kerning;
						FT_Get_Kerning(face, leftGlyph, glyph_index, FT_KERNING_DEFAULT, &kerning);
						advance += kerning.x;
					}
					pen_x += advance;
				}
				pen_y += lineHeight;
				++lineCpt;
			}

			break;
		}
	}
};
