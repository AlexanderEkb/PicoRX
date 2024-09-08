#ifndef _SURFACE_H_
#define _SURFACE_H_

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <cstdlib>

typedef union Pixel_t
{
  uint16_t raw;
  struct 
  {
    uint16_t r:5;
    uint16_t g:6;
    uint16_t b:5;
  };
} Pixel_t;

typedef struct Point_t
{
  uint32_t x;
  uint32_t y;
} Point_t;

typedef struct Rect_t
{
  uint32_t x;
  uint32_t y;
  uint32_t w;
  uint32_t h;
} Rect_t;

class Surface_t
{
  public:
    virtual uint32_t getWidth() = 0;
    virtual uint32_t getHeight() = 0;
    virtual Pixel_t * getPixels() = 0;
    virtual void blit(Surface_t * src, Rect_t from, Point_t to) = 0;
  protected:
    bool getIntersection(Rect_t & from, Point_t to)
    {
      if (to.x >= getWidth()) return false;
      if (to.y >= getHeight()) return false;
      if ((to.x + from.w) >= getWidth()) from.w = getWidth() - to.x;
      if ((to.y + from.h) >= getHeight()) from.h = getHeight() - to.y;
      return true;
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
        if (getIntersection(from, to) != true) return;
        Pixel_t * target = &pixels[to.y * width + to.x];
        for (uint32_t i=0; i<from.h; i++)
        {
          Pixel_t * source = &src->getPixels()[from.y * src->getWidth() + from.x];
          memcpy(target, source, sizeof(Pixel_t) * from.w);
        }
      }
  protected:
    uint32_t width;
    uint32_t height;
    Pixel_t * pixels;
};

#endif /* _SURFACE_H_ */