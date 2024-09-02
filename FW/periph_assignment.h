#ifndef _PERIPH_ASSIGNMENT_H_
#define _PERIPH_ASSIGNMENT_H_

#include "hardware/i2c.h"

// UI
static i2c_inst_t * const pI2CInstance      = i2c1;
static const uint8_t PIN_AB                 = 2;
static const uint8_t PIN_B                  = PIN_AB + 1;       // Strict requirement
static const uint8_t PIN_ENCODER_PUSH       = 4;
static const uint8_t PIN_DISPLAY_SDA        = 6;
static const uint8_t PIN_DISPLAY_SCL        = 7;
static const uint8_t PIN_MENU               = 9;
static const uint8_t PIN_BACK               = 10;

// RX
static const uint    NCO_PIN_0              = 0;
static const uint    NCO_PIN_1              = NCO_PIN_0 + 1;    // Strict requirement
static const uint    AUDIO_PIN              = 8;
static const uint    BAND_SEL_0             = 11;
static const uint    BAND_SEL_1             = 12;
static const uint    BAND_SEL_2             = 13;
static const uint    PSU_PIN                = 23;
static const uint    ADC_I                  = 26;
static const uint    ADC_Q                  = 27;
static const uint    ADC_BAT                = 29;


#endif /* _PERIPH_ASSIGNMENT_H_ */