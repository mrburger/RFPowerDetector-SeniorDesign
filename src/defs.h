#pragma once

/*-- Pins --*/

#define ADC_OUTA_CS_PIN 10 // CS Pin for ADC X. Later, we will have multiple ADCs, numbered. ACTIVE LOW
#define ADC_OUTB_CS_PIN 9
#define ADC_OUTN_CS_PIN 8 // Not complete. may need to use hardware CS pins, or a clever solution
#define ADC_OUTP_CS_PIN 7

/*-- Timings --*/
#define LED_BLINK_DELAY 750 // milliseconds

#define DISPLAY_ADDRESS 0x3C // 128x32 Display Address
#define DISPLAY_WIDTH 128 // X Pixels
#define DISPLAY_HEIGHT 32 // Y Pixels
#define FRAME_COUNT 3 // Number of animated frames
#define DISPLAY_BYTES 512 // Number of bytes in one frame

/*-- ADC Details --*/
#define ADC_SPI_TRANSFER_BYTE 0xFF // Equal to 255
#define ADC_MAX_BIT_VALUE (pow(2,14) - 1)

/*-- Enumerations & Structs --*/
// Each RF ADC has its own enumeration. This makes handling them easy
//const enum rfCSPins {outA = ADC_OUTA_CS_PIN, outB = ADC_OUTB_CS_PIN, outN = ADC_OUTN_CS_PIN, outP = ADC_OUTP_CS_PIN};

