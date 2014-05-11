# dfr - Dave's Font Renderer

## Installation
1. Download FreeType library http://www.freetype.org/
2. Setup your includes and binaries for FreeType
3. Download dfr.h and drf.cpp from this repository
4. Add those 2 files to your project
5. gg

## Quick tutorial
Call init
``` cpp
dfr::init();
```
Create your image buffer. Must be 32 bits per pixel -> RGBA
``` cpp
unsigned char* pImgData = new unsigned char [image_width * image_height * 4];
```
Initialize the image to all transparent
``` cpp
memset(pImgData, 0, texture_width * texture_height * 4);
```
Draw your text onto your image buffer
``` cpp
dfr::drawText("Hello World!", pImgData, image_width, image_height, "yourFontFile.ttf", 24);
```
Do what ever you please with your image buffer, like creating a texture:
``` cpp
Texture* pTexture = createTextureFromData(image_width, image_height, pImgData); // This is not part of dfr
```
Release resources
``` cpp
delete[] pImageData;
```

## Quick documentation
``` cpp
void dfr::drawText(
	const std::string& in_text,
	unsigned char* out_buffer, const unsigned int in_width, const unsigned int in_height,
	const std::string& in_font, const unsigned int in_size,
	bool in_wordWrap = true,
	const sColor& in_color = { 255, 255, 255 },
	eAlign in_align = ALIGN_TOP_LEFT,
	const unsigned int in_minSize = 0,
	int* out_containingRect = NULL);
```
Draw text using specific font and point size onto an image buffer.

**in_text** Text to render. i.e: "Hello World!"

**out_buffer** Your image buffer. This should be of size in_width * in_height * 4

**in_width** Width of your image

**in_height** Height of your image

**in_font** Font filename. i.e: "Content/Fonts/OpenSans-Semibold.ttf"

**in_size** Point size for the font.

**in_wordWrap** Enable multi lines if the text is too long

**in_color** RGB color for the text. Values are in range 0-255

**in_align** One of dfr::eAlign value.

**in_minSize** Minimum size allowed for the font. If the text doesn't fit in the image,
the renderer will pick a point size bellow and try again. Until this value is reached.
If a value of 0 is passed, autosize is disabled.

**out_containingRect** Array of 4 ints. Will be filled the containing rectangle of the text
rendered. x,y,w,h

**in_rightToLeft** For arabic text. This is experimental. But can get a rough job done
for simple labels
