#ifndef __UI_TYPES_H__
#define __UI_TYPES_H__

typedef union Color_t
{ 
  Color_t() : raw(0) {};
  Color_t(uint16_t color) : raw(color) {};
  uint16_t raw;
  struct 
  {
    uint16_t r:5;
    uint16_t g:6;
    uint16_t b:5;
  };
} Color_t;

typedef struct Point_t
{
  Point_t(uint32_t x, uint32_t y) : x(x), y(y) {};
  uint32_t x;
  uint32_t y;
} Point_t;

typedef struct Rect_t
{
  Rect_t(uint32_t x, uint32_t y, uint32_t w, uint32_t h) : x(x), y(y), w(w), h(h) {};
  uint32_t x;
  uint32_t y;
  uint32_t w;
  uint32_t h;
} Rect_t;

#endif /* __UI_TYPES_H__ */