#ifndef __FONTS_FONT_H__
#define __FONTS_FONT_H__

#include "../ui_types.h"

class Font_t
{
  public:
    virtual uint32_t firstCharacter() = 0;
    virtual uint32_t lastCharacter() = 0;
    virtual uint32_t getCharWidth(uint32_t chr) = 0;
    virtual uint32_t getBoxWidth(uint32_t chr) = 0;
    virtual uint32_t getFontHeight() = 0;
    virtual uint32_t getTextWidth(char * text)
    {
      uint32_t result = 0;
      while (*text) result += getBoxWidth(*text);
      return result;
    }
    virtual Pixel_t * getScanline(uint32_t chr, uint32_t scanline) = 0;
    virtual bool hasGlyph(uint32_t chr) = 0;
};

class MonoBitmapFont_t : public Font_t
{
  public:
    virtual Pixel_t * getScanline(uint32_t chr, uint32_t scanline)
    {
      if (!hasGlyph(chr)) chr = 0;
      if (chr != cachedCharacter)
      {
        cachedCharacter = chr;
        memset(reinterpret_cast<void *>(cache), 0, sizeof(Pixel_t) * getBoxWidth(' ') * getFontHeight());
        const uint32_t origin = (chr - firstCharacter()) * getCharWidth(chr);
        const uint32_t characterWidth = getCharWidth(chr);
        const uint32_t fontHeight = getFontHeight();
        const uint32_t boxWidth = getBoxWidth(chr);
        for (uint32_t col=0; col<characterWidth; col++)
        {
          uint8_t byte = getByte(origin + col);
          for (uint32_t row=0; row<fontHeight; row++)
          {
            cache[row * boxWidth + col].raw = (byte & 0x01) ? 0xFFFF : 0x0000;
            byte >>= 1;
          }
        }
      }

      const uint32_t origin = scanline * getBoxWidth(chr);
      return &cache[origin];
    }
  protected:
    uint32_t cachedCharacter;
    Pixel_t * cache;
    virtual uint8_t getByte(uint32_t index) = 0;
};

#endif /* __FONTS_FONT_H__ */