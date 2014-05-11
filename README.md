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
dfr::drawText(
	"Hello World!", 				// Text
	{ pImgData, image_width, image_height }, 	// Ouput image
	{ "Content/Fonts/OpenSans-Semibold.ttf", 24 }); // Font and point size
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
sRenderInfo drawText(
	const std::string& in_text,
	const sImage& in_outputImage,
	const sFont& in_font,
	const sFormating& in_formating = { },
	const sColor& in_color = { 255, 255, 255 });

```
Draw text using specific font and point size onto an image buffer.

**in_text** Text to render. i.e: "Hello World!"

**in_font** sFont structure defining the Font. Filename, pointsize

**in_outputImage** sImage structure defining the target image.

**in_formating** sFormating structure defining text formating, alignement, wordwrap and autosize.

**in_color** Color of the text RGB. Each component in range 0-255

**return:**
sRenderInfo structure containing information about the render. Rectangle, cursor position, etc.

