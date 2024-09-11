#ifndef _SURFACE_H_
#define _SURFACE_H_

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <cstdlib>
#include "ui_types.h"
#include "fonts/font.h"

class Surface_t
{
  public:
    virtual uint32_t getWidth() = 0;
    virtual uint32_t getHeight() = 0;
    virtual Color_t * getPixels() = 0;
    virtual void blit(Surface_t * src, Rect_t from, Point_t to) = 0;
    virtual void fill(Color_t color) = 0;
  protected:
    typedef enum CheckResult_t
    {
      CHECK_RESULT_FITS,
      CHECK_RESULT_SHRINKED,
      CHECK_RESULT_NO_INTERSECTION
    } CheckResult_t;
    CheckResult_t getIntersection(Rect_t & from, Point_t to)
    {
      bool shrinked = false;
      if (to.x >= getWidth()) return CHECK_RESULT_NO_INTERSECTION;
      if (to.y >= getHeight()) return CHECK_RESULT_NO_INTERSECTION;
      if ((to.x + from.w) > getWidth())
      {
        shrinked = true;
        from.w = getWidth() - to.x;
      }
      if ((to.y + from.h) > getHeight())
      {
        shrinked = true;
        from.h = getHeight() - to.y;
      }
      return shrinked ? CHECK_RESULT_SHRINKED : CHECK_RESULT_FITS;
    }
};

class Canvas_t : public Surface_t
{
  public:
    Canvas_t() :
      width(0),
      height(0),
      pixels(nullptr) {};
    Canvas_t(uint32_t w, uint32_t h) :
      width(w),
      height(h)
      {
        pixels = reinterpret_cast<Color_t *>(malloc(w * h * sizeof(Color_t)));
        assert(pixels);
      }
      virtual ~Canvas_t()
      {
        if (pixels != nullptr) free(pixels);
      }
      virtual uint32_t getWidth() {return width;};
      virtual uint32_t getHeight() {return height;};
      virtual Color_t * getPixels() {return pixels;};
      virtual void blit(Surface_t * src, Rect_t from, Point_t to)
      {
        if (getIntersection(from, to) == CHECK_RESULT_NO_INTERSECTION) return;
        Color_t * target = &pixels[to.y * width + to.x];
        for (uint32_t i=0; i<from.h; i++)
        {
          Color_t * source = &src->getPixels()[from.y * src->getWidth() + from.x];
          memcpy(target, source, sizeof(Color_t) * from.w);
        }
      }
      virtual void fill(Color_t color)
      {
        if(pixels != nullptr)
        {
          for (uint32_t i=0; i<width*height; i++)
            pixels[i].raw = color.raw;
        }
      }

      virtual void drawText(Font_t * font, uint32_t x, uint32_t y, char * text, Color_t color)
      {
        uint32_t textWidth = font->getTextWidth(text);
        uint32_t textHeight = font->getFontHeight();
        Rect_t textRect = Rect_t(x, y, textWidth, textHeight);
        if (getIntersection(textRect, Point_t(x, y)) == CHECK_RESULT_NO_INTERSECTION) return;

        while (*text)
        {
          uint32_t boxWidth = font->getBoxWidth(*text);
          for (uint32_t row=0; row<textHeight; row++)
          {
            uint32_t org = (y + row) * getWidth() + x;
            Color_t * scanline = font->getScanline(*text, row);
            for(uint32_t col=0; col<boxWidth; col++)
            {
              Color_t & alpha = scanline[col];
              
              Color_t * dest = &pixels[org + col];
              uint32_t  r = (dest->r * (32 - alpha.r) + color.r * alpha.r) >> 5;
              uint32_t  g = (dest->g * (64 - alpha.g) + color.g * alpha.g) >> 6;
              uint32_t  b = (dest->b * (32 - alpha.b) + color.b * alpha.b) >> 5;
              dest->r = r;
              dest->g = g;
              dest->b = b;
            }
          }
          x += boxWidth;
          text++;
        }
      }
  protected:
    uint32_t width;
    uint32_t height;
    Color_t * pixels;
};

#endif /* _SURFACE_H_ */