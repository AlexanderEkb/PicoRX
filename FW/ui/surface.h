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
    virtual Pixel_t * getPixels() = 0;
    virtual void blit(Surface_t * src, Rect_t from, Point_t to) = 0;
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
        pixels = reinterpret_cast<Pixel_t *>(malloc(w * h * sizeof(Pixel_t)));
        assert(pixels);
      }
      virtual ~Canvas_t()
      {
        if (pixels != nullptr) free(pixels);
      }
      uint32_t getWidth() {return width;};
      uint32_t getHeight() {return height;};
      Pixel_t * getPixels() {return pixels;};
      virtual void blit(Surface_t * src, Rect_t from, Point_t to)
      {
        if (getIntersection(from, to) == CHECK_RESULT_NO_INTERSECTION) return;
        Pixel_t * target = &pixels[to.y * width + to.x];
        for (uint32_t i=0; i<from.h; i++)
        {
          Pixel_t * source = &src->getPixels()[from.y * src->getWidth() + from.x];
          memcpy(target, source, sizeof(Pixel_t) * from.w);
        }
      }
      virtual void fill(Pixel_t color)
      {
        if(pixels != nullptr)
        {
          for (uint32_t i=0; i<width*height; i++)
            pixels[i].raw = color.raw;
        }
      }
  protected:
    uint32_t width;
    uint32_t height;
    Pixel_t * pixels;
};

#endif /* _SURFACE_H_ */