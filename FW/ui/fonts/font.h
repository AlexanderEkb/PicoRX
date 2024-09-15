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
    virtual uint32_t getTextWidth(char const * text)
    {
      uint32_t result = 0;
      while (*text)
      {
        result += getBoxWidth(*text);
        text++;
      }
      return result;
    }
    virtual Color_t * getScanline(uint32_t chr, uint32_t scanline) = 0;
    virtual bool hasGlyph(uint32_t chr) = 0;
};

class MonoBitmapFont_t : public Font_t
{
  public:
    virtual Color_t * getScanline(uint32_t chr, uint32_t scanline)
    {
      const bool hasNoGlyph = !hasGlyph(chr);
      const uint32_t boxWidth = getBoxWidth(chr);
      const uint32_t characterWidth = hasNoGlyph ? boxWidth : getCharWidth(chr);
      const uint32_t fontHeight = getFontHeight();
      if (hasNoGlyph) chr = 0;

      if (chr != cachedCharacter)
      {
        cachedCharacter = chr;
        memset(reinterpret_cast<void *>(cache), 0, sizeof(Color_t) * boxWidth * fontHeight);
        const uint32_t origin = (chr - firstCharacter()) * characterWidth;
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

      const uint32_t origin = scanline * boxWidth;
      return &cache[origin];
    }
  protected:
    uint32_t cachedCharacter;
    Color_t * cache;
    virtual uint8_t getByte(uint32_t index) = 0;
};

class Antialiased_4_BitmapFont_t : public Font_t
{
  public:
    virtual Color_t * getScanline(uint32_t chr, uint32_t scanline)
    {
      if (!hasGlyph(chr)) chr = '?';
      const uint32_t boxWidth = getBoxWidth(chr);
      const uint32_t characterWidth = getCharWidth(chr);
      const uint32_t fontHeight = getFontHeight();

      if (chr != cachedCharacter)
      {
        cachedCharacter = chr;
        memset(reinterpret_cast<void *>(cache), 0, sizeof(Color_t) * boxWidth * fontHeight);
        uint32_t index = (chr - firstCharacter()) * characterWidth * fontHeight * getBitsPerPixel() / 8;

        const uint32_t BITS_PER_BYTE = 8;
        const uint32_t BPP = getBitsPerPixel();
        const uint32_t DOTS_PER_BYTE = BITS_PER_BYTE / BPP;
        uint32_t rem = 0;
        Color_t buffer[DOTS_PER_BYTE];

        
        for (uint32_t col=0; col<characterWidth; col++)
        {
          for (uint32_t row=0; row<fontHeight; row++)
          {
            if (rem == 0)
            {
              uint8_t byte = getByte(index++);
              const uint8_t mask = (1 << BPP) - 1;
              static const uint32_t max_5 = ((1 << 5) - 1);
              static const uint32_t max_6 = ((1 << 5) - 1);
              static const uint32_t max_alpha = ((1 << BPP) - 1);
              for(uint32_t i=0; i<DOTS_PER_BYTE; i++)
              {
                const uint32_t alpha = byte & mask;
                const uint32_t alpha_5 = max_5 * alpha / max_alpha;
                const uint32_t alpha_6 = max_6 * alpha / max_alpha;

                buffer[i].r = alpha_5;
                buffer[i].g = alpha_6;
                buffer[i].b = alpha_5;
                byte >>= BPP;
              }

              rem = DOTS_PER_BYTE;
            }

            cache[row * boxWidth + (boxWidth - col) - 1].raw = buffer[--rem].raw;
          }
        }
      }

      const uint32_t origin = scanline * boxWidth;
      return &cache[origin];
    }
  protected:
    uint32_t cachedCharacter;
    Color_t * cache;
    virtual uint32_t getBitsPerPixel() = 0;
    virtual uint8_t getByte(uint32_t index) = 0;
};

#endif /* __FONTS_FONT_H__ */