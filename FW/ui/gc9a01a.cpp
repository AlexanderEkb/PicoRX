#include <string.h>
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "gc9a01a.h"
#include "pico/stdlib.h"

#include "font.h"

static uint8_t vBuf[240 * 240 * 2];

GC9A01A::GC9A01A(spi_inst_t * spi, uint32_t pin_sck, uint32_t pin_do, uint32_t pin_dc, uint32_t pin_cs)
{
  this->spi     = spi;
  this->pin_dc  = pin_dc;
  this->pin_cs  = pin_cs;
  this->pin_sck = pin_sck;
  this->pin_do  = pin_do;

  spi_init(spi, BAUDRATE);
  gpio_set_function(pin_do, GPIO_FUNC_SPI);
  gpio_set_function(pin_sck, GPIO_FUNC_SPI);
  gpio_set_function(pin_dc, GPIO_FUNC_SIO);
  gpio_set_function(pin_cs, GPIO_FUNC_SPI);

  gpio_set_dir(pin_dc, true);
  gpio_put(pin_dc, 1);

  dma_tx_channel = dma_claim_unused_channel(false);
  
  if (dma_tx_channel < 0) while(true);

  dma_tx_config = dma_channel_get_default_config(dma_tx_channel);

  channel_config_set_transfer_data_size(&dma_tx_config, DMA_SIZE_16);
  channel_config_set_dreq(&dma_tx_config, spi_get_index(spi) ? DREQ_SPI1_TX : DREQ_SPI0_TX);


  for(uint32_t i=0; i<240*240*2; i++)
    vBuf[i] = 0;
  writeInitSequence();
}

void GC9A01A::fill(uint16_t color)
{
  setViewport(0, 0, 239, 239);
  writeCommand(0x2C);
  startTransfer();
  for (uint32_t i=0; i<240*240/2; i++)
  {
    writeData32((color << 16) | color);
  }
}

void GC9A01A::flip(bool flip)
{

}

void GC9A01A::drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{

}

void GC9A01A::drawChar(uint32_t x, uint32_t y, uint32_t scale, const char c)
{
  const uint32_t chrHeiht = font_8x5[0];
  const uint32_t chrWidth = font_8x5[1];
  const uint32_t chrSpacing = font_8x5[2];
  const uint32_t chrOffset = font_8x5[3];
  const uint32_t bufWidth = chrWidth + chrSpacing;
  const uint32_t bufSize = chrHeiht * bufWidth;
  uint16_t buffer[bufSize];
  memset(buffer, 0, bufSize * sizeof(uint16_t));

  for (uint32_t col=0; col<chrWidth; col++)
  {
    uint8_t byte = font_8x5[(c - chrOffset) * chrWidth + col + 5];
    for (uint32_t row=0; row<chrHeiht; row++)
    {
      buffer[row * bufWidth + col] = (byte & 0x01) ? 0xFFFF : 0x0000;
      byte >>= 1;
    }
  }

  setViewport(
    x + 56, 
    y + 88,
    x + bufWidth + 55,
    y + chrHeiht + 87
  );
  writeCommand(0x2C);
  startTransfer();
  pushPixels(buffer, chrHeiht * bufWidth);
}

void GC9A01A::drawString(uint32_t x, uint32_t y, uint32_t scale, const char * s)
{
  const uint32_t step = font_8x5[1] + font_8x5[2];
  while(*s)
  {
    drawChar(x, y, scale, *s);
    x += step;
    s++;
  }
}

void GC9A01A::setViewport(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
  writeCommand(0x2A);
  writeData(static_cast<uint8_t>(x1 >> 8) & 0xFF);
  writeData(static_cast<uint8_t>(x1 >> 0) & 0xFF);
  writeData(static_cast<uint8_t>(x2 >> 8) & 0xFF);
  writeData(static_cast<uint8_t>(x2 >> 0) & 0xFF);

  writeCommand(0x2B);
  writeData(static_cast<uint8_t>(y1 >> 8) & 0xFF);
  writeData(static_cast<uint8_t>(y1 >> 0) & 0xFF);
  writeData(static_cast<uint8_t>(y2 >> 8) & 0xFF);
  writeData(static_cast<uint8_t>(y2 >> 0) & 0xFF);
}

void GC9A01A::writeCommand(uint8_t cmd)
{
  gpio_put(pin_dc, 0);
  spi_write_blocking(spi, &cmd, 1);
}

void GC9A01A::writeData(uint8_t data)
{
  gpio_put(pin_dc, 1);
  spi_write_blocking(spi, &data, 1);
}

void GC9A01A::startTransfer()
{
  gpio_put(pin_dc, 1);
}

void GC9A01A::dmaWait()
{
  while (dma_channel_is_busy(dma_tx_channel));
  // For SPI must also wait for FIFO to flush and reset format
  while (spi_get_hw(spi)->sr & SPI_SSPSR_BSY_BITS) {};
}

void GC9A01A::pushPixelsDMA(uint16_t* image, uint32_t len)
{
  if (len == 0) return;
  dmaWait();
  channel_config_set_bswap(&dma_tx_config, false);
  dma_channel_configure(dma_tx_channel, &dma_tx_config, &spi_get_hw(spi)->dr, (uint16_t*)image, len, true);
}

void GC9A01A::pushPixels(uint16_t* image, uint32_t len)
{
  if (len == 0) return;
  spi_write_blocking(spi, (uint8_t *)image, len*2);
}

void GC9A01A::pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t* image, uint16_t* buffer)
{
  // if ((x >= _vpW) || (y >= _vpH)) return;

  // int32_t dx = 0;
  // int32_t dy = 0;
  // int32_t dw = w;
  // int32_t dh = h;

  // if (x < _vpX) { dx = _vpX - x; dw -= dx; x = _vpX; }
  // if (y < _vpY) { dy = _vpY - y; dh -= dy; y = _vpY; }

  // if ((x + dw) > _vpW ) dw = _vpW - x;
  // if ((y + dh) > _vpH ) dh = _vpH - y;

  // if (dw < 1 || dh < 1) return;

  // uint32_t len = dw*dh;

  // if (buffer == nullptr) {
  //   buffer = image;
  //   dmaWait();
  // }

  // // If image is clipped, copy pixels into a contiguous block
  // if ( (dw != w) || (dh != h) ) {
  //   for (int32_t yb = 0; yb < dh; yb++) {
  //     memmove((uint8_t*) (buffer + yb * dw), (uint8_t*) (image + dx + w * (yb + dy)), dw << 1);
  //   }
  // }
  // // else, if a buffer pointer has been provided copy whole image to the buffer
  // else if (buffer != image) {
  //   memcpy(buffer, image, len*2);
  // }

  // dmaWait(); // In case we did not wait earlier

  // setAddrWindow(x, y, dw, dh);

  // channel_config_set_bswap(&dma_tx_config, false);

  // dma_channel_configure(dma_tx_channel, &dma_tx_config, &spi_get_hw(SPI_X)->dr, (uint16_t*)buffer, len, true);
}

void GC9A01A::writeData16(uint32_t data)
{
  spi_write_blocking(spi, reinterpret_cast<uint8_t *>(&data), 2);
}

void GC9A01A::writeData32(uint32_t data)
{
  spi_write_blocking(spi, reinterpret_cast<uint8_t *>(&data), 4);
}

void GC9A01A::writeInitSequence()
{
  writeCommand(0xEF);   // Inter register 2
  writeCommand(0xEB);   // ??? Unknown
  writeData(0x14);

  writeCommand(0xFE);   // Inter register 1
  writeCommand(0xEF);   // Inter register 2

  writeCommand(0xEB);   // ??? Unknown
  writeData(0x14);

  writeCommand(0x84);   // ??? Unknown
  writeData(0x40);

  writeCommand(0x85);   // ??? Unknown
  writeData(0xFF);

  writeCommand(0x86);   // ??? Unknown
  writeData(0xFF);

  writeCommand(0x87);   // ??? Unknown
  writeData(0xFF);

  writeCommand(0x88);   // ??? Unknown
  writeData(0x0A);

  writeCommand(0x89);   // ??? Unknown
  writeData(0x21);

  writeCommand(0x8A);   // ??? Unknown
  writeData(0x00);

  writeCommand(0x8B);   // ??? Unknown
  writeData(0x80);

  writeCommand(0x8C);   // ??? Unknown
  writeData(0x01);

  writeCommand(0x8D);   // ??? Unknown
  writeData(0x01);

  writeCommand(0x8E);   // ??? Unknown
  writeData(0xFF);

  writeCommand(0x8F);   // ??? Unknown
  writeData(0xFF);

  writeCommand(0xB6);   // Display Function Control
  writeData(0x00);
  writeData(0x20);      // Reverse source scan direction, forward gate scan direction

  writeCommand(0x3A);   // Pixel format
  writeData(0x05);      // 16 bit/pixel

  writeCommand(0x90);   // ??? Unknown
  writeData(0x08);
  writeData(0x08);
  writeData(0x08);
  writeData(0x08);

  writeCommand(0xBD);   // ??? Unknown
  writeData(0x06);

  writeCommand(0xBC);   // ??? Unknown
  writeData(0x00);

  writeCommand(0xFF);   // ??? Unknown
  writeData(0x60);
  writeData(0x01);
  writeData(0x04);

  writeCommand(0xC3);   // Power control 2
  writeData(0x13);      // VREG1A = 5.18V,  VRGE1B = 0.68V
  writeCommand(0xC4);   // Power control 3
  writeData(0x13);      // VREG2A = -3.82V  VREG2B = 0.41V

  writeCommand(0xC9);   // Power control 4
  writeData(0x22);      // VRGE1A = 5.88V   VREG2A = -2.88V

  writeCommand(0xBE);   // ??? Unknown
  writeData(0x11);

  writeCommand(0xE1);   // ??? Unknown
  writeData(0x10);
  writeData(0x0E);

  writeCommand(0xDF);   // ??? Unknown
  writeData(0x21);
  writeData(0x0c);
  writeData(0x02);

  writeCommand(0xF0);   // Gamma 1
  writeData(0x45);
  writeData(0x09);
  writeData(0x08);
  writeData(0x08);
  writeData(0x26);
  writeData(0x2A);

  writeCommand(0xF1);   // Gamma 2
  writeData(0x43);
  writeData(0x70);
  writeData(0x72);
  writeData(0x36);
  writeData(0x37);
  writeData(0x6F);

  writeCommand(0xF2);   // Gamma 3
  writeData(0x45);
  writeData(0x09);
  writeData(0x08);
  writeData(0x08);
  writeData(0x26);
  writeData(0x2A);

  writeCommand(0xF3);   // Gamma 4
  writeData(0x43);
  writeData(0x70);
  writeData(0x72);
  writeData(0x36);
  writeData(0x37);
  writeData(0x6F);

  writeCommand(0xED);   // ??? Unknown
  writeData(0x1B);
  writeData(0x0B);

  writeCommand(0xAE);   // ??? Unknown
  writeData(0x77);

  writeCommand(0xCD);   // ??? Unknown
  writeData(0x63);

  writeCommand(0x70);   // ??? Unknown
  writeData(0x07);
  writeData(0x07);
  writeData(0x04);
  writeData(0x0E);
  writeData(0x0F);
  writeData(0x09);
  writeData(0x07);
  writeData(0x08);
  writeData(0x03);

  writeCommand(0xE8);   // Frame rate (???)
  writeData(0x34);      // Unclear

  writeCommand(0x62);   // ??? Unknown
  writeData(0x18);
  writeData(0x0D);
  writeData(0x71);
  writeData(0xED);
  writeData(0x70);
  writeData(0x70);
  writeData(0x18);
  writeData(0x0F);
  writeData(0x71);
  writeData(0xEF);
  writeData(0x70);
  writeData(0x70);

  writeCommand(0x63);   // ??? Unknown
  writeData(0x18);
  writeData(0x11);
  writeData(0x71);
  writeData(0xF1);
  writeData(0x70);
  writeData(0x70);
  writeData(0x18);
  writeData(0x13);
  writeData(0x71);
  writeData(0xF3);
  writeData(0x70);
  writeData(0x70);

  writeCommand(0x64);   // ??? Unknown
  writeData(0x28);
  writeData(0x29);
  writeData(0xF1);
  writeData(0x01);
  writeData(0xF1);
  writeData(0x00);
  writeData(0x07);

  writeCommand(0x66);   // ??? Unknown
  writeData(0x3C);
  writeData(0x00);
  writeData(0xCD);
  writeData(0x67);
  writeData(0x45);
  writeData(0x45);
  writeData(0x10);
  writeData(0x00);
  writeData(0x00);
  writeData(0x00);

  writeCommand(0x67);   // ??? Unknown
  writeData(0x00);
  writeData(0x3C);
  writeData(0x00);
  writeData(0x00);
  writeData(0x00);
  writeData(0x01);
  writeData(0x54);
  writeData(0x10);
  writeData(0x32);
  writeData(0x98);

  writeCommand(0x74);   // ??? Unknown
  writeData(0x10);
  writeData(0x85);
  writeData(0x80);
  writeData(0x00);
  writeData(0x00);
  writeData(0x4E);
  writeData(0x00);

  writeCommand(0x98);   // ??? Unknown
  writeData(0x3e);
  writeData(0x07);

  writeCommand(0x35);   // Tearing Effect Line ON, but it requires a parameter...
  writeCommand(0x21);   // Display Inversion ON

  writeCommand(0x11);   // Out Of Sleep
  sleep_us(120000);
  writeCommand(0x29);   // Display ON
  sleep_us(20000);
}
