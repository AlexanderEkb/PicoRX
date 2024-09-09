#ifndef _GC9A01A_H_
#define _GC9A01A_H_

#include "hardware/spi.h"
#include "hardware/dma.h"
#include "surface.h"

class GC9A01A : public Surface_t
{
  public:
    static const uint32_t DISPLAY_WIDTH = 240;
    static const uint32_t DISPLAY_HEIGHT = 240;
    GC9A01A(spi_inst_t * spi, uint32_t pin_sck, uint32_t pin_do, uint32_t pin_dc, uint32_t pin_cs);

    virtual uint32_t getWidth() {return DISPLAY_WIDTH;};
    virtual uint32_t getHeight() {return DISPLAY_HEIGHT;};
    virtual Pixel_t * getPixels() {return nullptr;};
    virtual void blit(Surface_t * src, Rect_t from, Point_t to);

    void setViewport(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
    void fill(uint16_t color);
    void flip(bool flip);
    void drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint16_t color);
    void drawChar(uint32_t x, uint32_t y, uint32_t scale, const char c);
    void drawString(uint32_t x, uint32_t y, uint32_t scale, const char * s);
  private:
    static const uint32_t BAUDRATE = 50000000;
    spi_inst_t * spi;
    uint32_t pin_dc;
    uint32_t pin_cs;
    uint32_t pin_do;
    uint32_t pin_sck;

    int32_t            dma_tx_channel;
    dma_channel_config dma_tx_config;

    void writeCommandEx(uint8_t command, uint32_t count, ...);
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void startTransfer();
    void dmaWait();
    void pushPixelsDMA(Pixel_t * image, uint32_t len);
    void writeData32(uint32_t data);
    void writeInitSequence();
};

#endif /* _GC9A01A_H_ */