#ifndef _GC9A01A_H_
#define _GC9A01A_H_

#include "hardware/spi.h"
#include "hardware/dma.h"

class GC9A01A
{
  public:
    GC9A01A(spi_inst_t * spi, uint32_t pin_sck, uint32_t pin_do, uint32_t pin_dc, uint32_t pin_cs);
    void setViewport(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
    void fill(uint16_t color);
    void flip(bool flip);
    void drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
    void drawChar(uint32_t x, uint32_t y, uint32_t scale, const char c);
    void drawString(uint32_t x, uint32_t y, uint32_t scale, const char * s);
  private:
    static const uint32_t BAUDRATE = 40000000;
    spi_inst_t * spi;
    uint32_t pin_dc;
    uint32_t pin_cs;
    uint32_t pin_do;
    uint32_t pin_sck;

    int32_t            dma_tx_channel;
    dma_channel_config dma_tx_config;

    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void startTransfer();
    void dmaWait();
    void pushPixels(uint16_t* image, uint32_t len);
    void pushPixelsDMA(uint16_t* image, uint32_t len);
    void pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t* image, uint16_t* buffer);
    void writeData16(uint32_t data);
    void writeData32(uint32_t data);
    void writeInitSequence();
};

#endif /* _GC9A01A_H_ */