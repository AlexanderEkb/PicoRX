#ifndef _PERIPH_ASSIGNMENT_H_
#define _PERIPH_ASSIGNMENT_H_

#include "hardware/i2c.h"
#include "hardware/spi.h"

// UI
static i2c_inst_t * const pI2CInstance      = i2c1;
static spi_inst_t * const pSPIInstance      = spi0;

static const uint8_t PIN_AB                 = 2;
static const uint8_t PIN_B                  = PIN_AB + 1;       // Strict requirement

static const uint8_t PIN_DISPLAY_DC         = 4;
static const uint8_t PIN_DISPLAY_CS         = 5;
static const uint8_t PIN_DISPLAY_SCK        = 6;
static const uint8_t PIN_DISPLAY_DO         = 7;

static const uint8_t PIN_ENCODER_PUSH       = 8;
static const uint8_t PIN_MENU               = 9;
static const uint8_t PIN_BACK               = 10;

static const uint8_t PIN_DISPLAY_SDA        = 6;
static const uint8_t PIN_DISPLAY_SCL        = 7;

// static const uint8_t PIN_ENCODER_PUSH       = 4;
// static const uint8_t PIN_DISPLAY_SDA        = 6;
// static const uint8_t PIN_DISPLAY_SCL        = 7;
// static const uint8_t PIN_MENU               = 9;
// static const uint8_t PIN_BACK               = 10;

// RX
static const uint    NCO_PIN_0              = 0;
static const uint    NCO_PIN_1              = NCO_PIN_0 + 1;    // Strict requirement
static const uint    AUDIO_PIN              = 11;
static const uint    BAND_SEL_0             = 16;               // Not routed
static const uint    BAND_SEL_1             = 17;               // Not routed
static const uint    BAND_SEL_2             = 18;               // Not routed
static const uint    PSU_PIN                = 23;               // Actually not required
static const uint    ADC_I                  = 26;
static const uint    ADC_Q                  = 27;
static const uint    ADC_BAT                = 29;

/*
 Control panel pinout:
 =====================

 1  x
 2  x
 3  x
 4  IO10    Back
 5  IO9     Menu
 6  IO8     Recall
 7  IO7     Display DO
 8  IO6     Display SCK
 9  IO5     Display CS
 10 IO4     Display DC
 11 IO3     Encoder B
 12 IO2     Encoder A
 13 GND
 14 3V3

*/

#endif /* _PERIPH_ASSIGNMENT_H_ */