#ifndef __LOGO_H__
#define __LOGO_H__

#include "surface.h"

class Logo_t : public Surface_t
{
  public:
    virtual ~Logo_t() {};
    virtual uint32_t getWidth() override {return WIDTH;};
    virtual uint32_t getHeight() override {return HEIGHT;};
    virtual Color_t * getPixels() override;
    virtual void blit(Surface_t * src, Rect_t from, Point_t to) {/* Do nothing */};
    virtual void fill(Color_t color) {/* Do nothing */};
  private:
    static const uint32_t WIDTH = 240;
    static const uint32_t HEIGHT = 239;
};

#endif /* __LOGO_H__ */