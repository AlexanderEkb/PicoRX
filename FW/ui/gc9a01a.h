#ifndef _GC9A01A_H_
#define _GC9A01A_H_

#include "hardware/spi.h"

class GC9A01A
{
  public:
    GC9A01A(spi_inst_t * spi, uint32_t pin_sck, uint32_t pin_do, uint32_t pin_dc, uint32_t pin_cs);
  private:
    static const uint32_t BAUDRATE = 40000000;
    spi_inst_t * spi;
    uint32_t pin_dc;
    uint32_t pin_cs;
    uint32_t pin_do;
    uint32_t pin_sck;
    void writecommand(uint8_t cmd);
    void writedata(uint8_t data);
    void writeInitSequence();
};

#endif /* _GC9A01A_H_ */