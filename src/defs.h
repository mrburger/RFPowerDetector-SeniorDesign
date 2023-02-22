#pragma once
#include <Arduino.h> // Basic Arduino Library
#include <iostream>
#include <WString.h> // Teensy String Library
#include <vector>



/*-- Pins --*/
#define ADC_OUTA_CS_PIN 9 // CS Pin for ADC X. Later, we will have multiple ADCs, numbered. ACTIVE LOW
#define ADC_OUTB_CS_PIN 8
#define ADC_OUTN_CS_PIN 7 // Not complete. may need to use hardware CS pins, or a clever solution
#define ADC_OUTP_CS_PIN 6

/*-- Timings --*/
#define LED_BLINK_DELAY 750 // milliseconds
#define RF_SAMPLE_INTERVAL 1E6 // microseconds

#define DISPLAY_ADDRESS 0x3C // 128x32 Display Address
#define DISPLAY_WIDTH 128 // X Pixels
#define DISPLAY_HEIGHT 32 // Y Pixels
#define FRAME_COUNT 3 // Number of animated frames
#define DISPLAY_BYTES 512 // Number of bytes in one frame

/*-- ADC Details --*/
#define ADC_SPI_TRANSFER_BYTE 0xFF // Equal to 255
#define ADC_BITS 14
#define ADC_MAX_BIT_VALUE (pow(2, ADC_BITS) - 1)
#define SPI_CLK_SPEED 100E6 // 100 MHz, maximum speed
#define ADC_MAX_VOLTAGE 5.0 // ADC Reference maximum supply

/* -- Data Structures --*/
#define RF_INVALID_VALUE -256.0

/*-- Enumerations & Structs --*/
// Each RF ADC has its own enumeration. This makes handling them easy
//const enum rfCSPins {outA = ADC_OUTA_CS_PIN, outB = ADC_OUTB_CS_PIN, outN = ADC_OUTN_CS_PIN, outP = ADC_OUTP_CS_PIN};
#define RAM_MAXIMUM 8E6 // 8MB ram
#define SAMPLE_DATA_SIZE (4 * 6) + 2 // 4 Bytes, 4 Values. 2 bytes extra for necessary stuff. TODO: Make this an actual value
#define RF_BUFFER_LENGTH (int) (RAM_MAXIMUM / SAMPLE_DATA_SIZE) // MUST Be less than 8MB of ram.
#define FILLARRAY(a,n) a[0]=n, memcpy( ((char*)a)+sizeof(a[0]), a, sizeof(a)-sizeof(a[0]) );
enum EnumADC {adcA, adcB, adcN, adcP};

/*-- Strings and Formatting Values --*/
#define VOLTAGE_SYMBOL "V" // Volts SI symbol
#define AMPERAGE_SYMBOL "A"
#define MILLI_PREFIX_SYMBOL "m" // milli SI prefix, 1E-3
#define MILLI_SCALE_FACTOR 1E3 // 1.0 is 1000 milli
#define MEASUREMENT_DECIMALS 2 // How many decimal places to display by default