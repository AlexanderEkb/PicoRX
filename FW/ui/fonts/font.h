#ifndef __FONTS_FONT_H__
#define __FONTS_FONT_H__

#include "../ui_types.h"

class Font_t
{
  public:
    virtual int getCharWidth(uint32_t chr);
    virtual int getFontHeight();
    virtual void getScanline(uint32_t chr, uint32_t scanline, Pixel_t * dest);
};

class RasterFont_t : public Font_t
{
  public:
  protected:
};

#endif /* __FONTS_FONT_H__ */