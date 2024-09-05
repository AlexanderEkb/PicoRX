#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "gc9a01a.h"
#include "pico/stdlib.h"

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
  gpio_set_function(pin_cs, GPIO_FUNC_SIO);

  gpio_set_dir(pin_dc, true);
  gpio_set_dir(pin_cs, true);

  gpio_put(pin_dc, 1);
  gpio_put(pin_cs, 1);

  for(uint32_t i=0; i<240*240*2; i++)
    vBuf[i] = 0;
  writeInitSequence();
}

void GC9A01A::writecommand(uint8_t cmd)
{
  gpio_put(pin_dc, 0);
  gpio_put(pin_cs, 0);
  spi_write_blocking(spi, &cmd, 1);
  gpio_put(pin_cs, 1);
}

void GC9A01A::writedata(uint8_t data)
{
  gpio_put(pin_dc, 1);
  gpio_put(pin_cs, 0);
  spi_write_blocking(spi, &data, 1);
  gpio_put(pin_cs, 1);
}

void GC9A01A::writeInitSequence()
{
  writecommand(0xEF);   // Inter register 2
  writecommand(0xEB);   // ??? Unknown
  writedata(0x14);

  writecommand(0xFE);   // Inter register 1
  writecommand(0xEF);   // Inter register 2

  writecommand(0xEB);   // ??? Unknown
  writedata(0x14);

  writecommand(0x84);   // ??? Unknown
  writedata(0x40);

  writecommand(0x85);   // ??? Unknown
  writedata(0xFF);

  writecommand(0x86);   // ??? Unknown
  writedata(0xFF);

  writecommand(0x87);   // ??? Unknown
  writedata(0xFF);

  writecommand(0x88);   // ??? Unknown
  writedata(0x0A);

  writecommand(0x89);   // ??? Unknown
  writedata(0x21);

  writecommand(0x8A);   // ??? Unknown
  writedata(0x00);

  writecommand(0x8B);   // ??? Unknown
  writedata(0x80);

  writecommand(0x8C);   // ??? Unknown
  writedata(0x01);

  writecommand(0x8D);   // ??? Unknown
  writedata(0x01);

  writecommand(0x8E);   // ??? Unknown
  writedata(0xFF);

  writecommand(0x8F);   // ??? Unknown
  writedata(0xFF);

  writecommand(0xB6);   // Display Function Control
  writedata(0x00);
  writedata(0x20);      // Reverse source scan direction, forward gate scan direction

  writecommand(0x3A);   // Pixel format
  writedata(0x05);      // 16 bit/pixel

  writecommand(0x90);   // ??? Unknown
  writedata(0x08);
  writedata(0x08);
  writedata(0x08);
  writedata(0x08);

  writecommand(0xBD);   // ??? Unknown
  writedata(0x06);

  writecommand(0xBC);   // ??? Unknown
  writedata(0x00);

  writecommand(0xFF);   // ??? Unknown
  writedata(0x60);
  writedata(0x01);
  writedata(0x04);

  writecommand(0xC3);   // Power control 2
  writedata(0x13);      // VREG1A = 5.18V,  VRGE1B = 0.68V
  writecommand(0xC4);   // Power control 3
  writedata(0x13);      // VREG2A = -3.82V  VREG2B = 0.41V

  writecommand(0xC9);   // Power control 4
  writedata(0x22);      // VRGE1A = 5.88V   VREG2A = -2.88V

  writecommand(0xBE);   // ??? Unknown
  writedata(0x11);

  writecommand(0xE1);   // ??? Unknown
  writedata(0x10);
  writedata(0x0E);

  writecommand(0xDF);   // ??? Unknown
  writedata(0x21);
  writedata(0x0c);
  writedata(0x02);

  writecommand(0xF0);   // Gamma 1
  writedata(0x45);
  writedata(0x09);
  writedata(0x08);
  writedata(0x08);
  writedata(0x26);
  writedata(0x2A);

  writecommand(0xF1);   // Gamma 2
  writedata(0x43);
  writedata(0x70);
  writedata(0x72);
  writedata(0x36);
  writedata(0x37);
  writedata(0x6F);

  writecommand(0xF2);   // Gamma 3
  writedata(0x45);
  writedata(0x09);
  writedata(0x08);
  writedata(0x08);
  writedata(0x26);
  writedata(0x2A);

  writecommand(0xF3);   // Gamma 4
  writedata(0x43);
  writedata(0x70);
  writedata(0x72);
  writedata(0x36);
  writedata(0x37);
  writedata(0x6F);

  writecommand(0xED);   // ??? Unknown
  writedata(0x1B);
  writedata(0x0B);

  writecommand(0xAE);   // ??? Unknown
  writedata(0x77);

  writecommand(0xCD);   // ??? Unknown
  writedata(0x63);

  writecommand(0x70);   // ??? Unknown
  writedata(0x07);
  writedata(0x07);
  writedata(0x04);
  writedata(0x0E);
  writedata(0x0F);
  writedata(0x09);
  writedata(0x07);
  writedata(0x08);
  writedata(0x03);

  writecommand(0xE8);   // Frame rate (???)
  writedata(0x34);      // Unclear

  writecommand(0x62);   // ??? Unknown
  writedata(0x18);
  writedata(0x0D);
  writedata(0x71);
  writedata(0xED);
  writedata(0x70);
  writedata(0x70);
  writedata(0x18);
  writedata(0x0F);
  writedata(0x71);
  writedata(0xEF);
  writedata(0x70);
  writedata(0x70);

  writecommand(0x63);   // ??? Unknown
  writedata(0x18);
  writedata(0x11);
  writedata(0x71);
  writedata(0xF1);
  writedata(0x70);
  writedata(0x70);
  writedata(0x18);
  writedata(0x13);
  writedata(0x71);
  writedata(0xF3);
  writedata(0x70);
  writedata(0x70);

  writecommand(0x64);   // ??? Unknown
  writedata(0x28);
  writedata(0x29);
  writedata(0xF1);
  writedata(0x01);
  writedata(0xF1);
  writedata(0x00);
  writedata(0x07);

  writecommand(0x66);   // ??? Unknown
  writedata(0x3C);
  writedata(0x00);
  writedata(0xCD);
  writedata(0x67);
  writedata(0x45);
  writedata(0x45);
  writedata(0x10);
  writedata(0x00);
  writedata(0x00);
  writedata(0x00);

  writecommand(0x67);   // ??? Unknown
  writedata(0x00);
  writedata(0x3C);
  writedata(0x00);
  writedata(0x00);
  writedata(0x00);
  writedata(0x01);
  writedata(0x54);
  writedata(0x10);
  writedata(0x32);
  writedata(0x98);

  writecommand(0x74);   // ??? Unknown
  writedata(0x10);
  writedata(0x85);
  writedata(0x80);
  writedata(0x00);
  writedata(0x00);
  writedata(0x4E);
  writedata(0x00);

  writecommand(0x98);   // ??? Unknown
  writedata(0x3e);
  writedata(0x07);

  writecommand(0x35);   // Tearing Effect Line ON, but it requires a parameter...
  writecommand(0x21);   // Display Inversion ON

  writecommand(0x11);   // Out Of Sleep
  sleep_us(120000);
  writecommand(0x29);   // Display ON
  sleep_us(20000);
}
