dfr - Dave's Font Renderer

=== Installation

1. Download FreeType library http://www.freetype.org/
2. Setup your includes and binaries for FreeType
3. Download dfr.h and drf.cpp from this repository
4. Add those 2 files to your project
5. gg

=== Quick tutorial

1. Call init

dfr::init();

2. Create your image buffer. Must be 32 bits per pixel -> RGBA

unsigned char* pImgData = new unsigned char [image_width * image_height * 4];

3. Initialize the image to all transparent

memset(pImgData, 0, texture_width * texture_height * 4);

4. Draw your text onto your image buffer

dfr::drawText("Hello World!", pImgData, image_width, image_height, "yourFontFile.ttf", 24);

5. Do what ever you please with your image buffer, like creating a texture:

Texture* pTexture = createTextureFromData(image_width, image_height, pImgData); // This is not part of dfr

6. Release resources

delete[] pImageData;

=== Quick documentation

dfr::drawText();

