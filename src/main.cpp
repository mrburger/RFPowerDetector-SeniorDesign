#include <Arduino.h>
#include <defs.h> // Constant Definitions

#include <Metro.h> // Task scheduler

#include <i2c_device.h> // Teensy 4 I2C Library
#include <Adafruit_GFX.h> // Graphics Lib
#include <Adafruit_SSD1306.h> // Display Lib


/*-- Constants --*/
const RFInputADC adcOutA = RFInputADC(ADC_OUTA_CS_PIN);
const RFInputADC adcOutB = RFInputADC(ADC_OUTB_CS_PIN);
const RFInputADC adcOutN = RFInputADC(ADC_OUTN_CS_PIN);
const RFInputADC adcOutP = RFInputADC(ADC_OUTP_CS_PIN);
RFInputADC rfADCArray[] = {adcOutA, adcOutB, adcOutN, adcOutP};

/*-- Function Definition --*/
void toggleLED();
void takeMeasurements();
uint16_t getADCMeasure(RFInputADC selectedADC);

/*-- Constructors --*/
Metro blinkTimer = Metro(LED_BLINK_DELAY); // Idle status LED Blink
Metro serialOutputTimer = Metro(1E3); // TODO: Make this prettier.
Metro invertTimer = Metro(1E3); // 1 Second Change
Metro animationTimer = Metro(250); // 500 ms
//Adafruit_SSD1306 display = Adafruit_SSD1306(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire2); // Working I2C T4.1 Display
IntervalTimer sampleTimer; 

/*-- Variables --*/
SPISettings spiADCSettings(SPI_CLK_SPEED, MSBFIRST, SPI_MODE0); // MCP33151 Maximums
uint16_t directADCReadValue; // Prototyping ADC value. TODO: translate into voltage
uint64_t directADCCounter = 0; // Counts number of measurements in a window
uint64_t directADCSum = 0;
float directRollingAverage = 0.0;
EXTMEM RFSample bigAssBuffer[RF_BUFFER_LENGTH];
RFSample currentSample;

bool displayInverted = false; // Inversion storage value
uint8_t currentFrame = 0;
// TODO: move these to the flash memory
static const uint8_t PROGMEM frame1_data[DISPLAY_BYTES] = 
{0x12, 0x8f, 0xd6, 0x6a, 0x78, 0xaf, 0xb3, 0x54, 0x59, 0xac, 0xc6, 0x19, 0x6b, 0xbb, 0xef, 0x25, 0x95, 0x40, 0xfe, 0x95, 0xbe, 0x26, 0xb9, 0xa3, 0x1b, 0x44, 0xd8, 0xc, 0x4e, 0xa2, 0xb1, 0xbd, 0xa2, 0x59, 0xba, 0x17, 0x31, 0x61, 0xe, 0x1, 0x4, 0x6a, 0x20, 0x8d, 0x82, 0xaa, 0x62, 0x41, 0x53, 0x13, 0x23, 0x55, 0xec, 0x55, 0x8d, 0xb8, 0xfd, 0xc8, 0x1, 0xf9, 0xa0, 0x38, 0xff, 0xce, 0xee, 0xa4, 0xce, 0x2c, 0xb8, 0xf5, 0x63, 0x68, 0x6b, 0x7e, 0x62, 0x21, 0xb0, 0x33, 0x3f, 0xc9, 0xd, 0x77, 0x56, 0xcb, 0x82, 0xed, 0xe8, 0xb0, 0xfb, 0xe4, 0x29, 0xb0, 0x7d, 0x2b, 0xf, 0x46, 0xac, 0xba, 0x28, 0x17, 0xd4, 0x85, 0xa2, 0x75, 0xad, 0xa8, 0x5c, 0x38, 0xa4, 0xd, 0x2e, 0xd8, 0x47, 0x7b, 0x2c, 0x5f, 0xfa, 0x7c, 0x33, 0xb3, 0x74, 0xaf, 0x6d, 0x1d, 0x8a, 0x35, 0x42, 0x13, 0xf8, 0xd4, 0xc7, 0xc3, 0x7e, 0x3c, 0xbd, 0xab, 0x96, 0xa0, 0x96, 0x70, 0x17, 0x1, 0x2f, 0xcb, 0xcb, 0xbd, 0xdc, 0xee, 0x4f, 0x24, 0xa4, 0xc6, 0x28, 0xf, 0x5b, 0x36, 0x3b, 0x85, 0x26, 0x6, 0xb7, 0xdb, 0xe3, 0x15, 0xfe, 0x23, 0xfd, 0x34, 0x7e, 0xe4, 0x39, 0xdd, 0xfe, 0xa6, 0x21, 0xc4, 0x76, 0x17, 0x8a, 0xa7, 0xbc, 0x2f, 0x21, 0x16, 0x24, 0xf4, 0x66, 0x86, 0x23, 0xf9, 0x9a, 0xca, 0xa0, 0x2e, 0x60, 0x94, 0x5f, 0xc, 0xfa, 0x0, 0x1e, 0x2a, 0xb3, 0x36, 0x1a, 0xd2, 0x9b, 0x18, 0x72, 0x3a, 0xca, 0xdb, 0x41, 0x28, 0x1f, 0xbe, 0xcb, 0xb, 0x73, 0x3, 0x92, 0x6c, 0x9c, 0x9f, 0x53, 0x52, 0x60, 0x79, 0xa8, 0xcc, 0xf7, 0x67, 0xe2, 0x0, 0x5, 0x4c, 0x15, 0x91, 0xd5, 0xa7, 0xc9, 0x36, 0x3b, 0x1e, 0xe, 0xa3, 0xe2, 0x6e, 0x49, 0x47, 0x27, 0xa4, 0xfa, 0x11, 0x14, 0xd5, 0xf9, 0x98, 0x12, 0x65, 0x84, 0x66, 0xd7, 0x85, 0x5c, 0x54, 0xc0, 0x3c, 0x35, 0xa2, 0x79, 0xe8, 0xca, 0x11, 0x26, 0xfa, 0xe4, 0x82, 0xaa, 0xe8, 0x84, 0xa9, 0xfe, 0xaa, 0x4, 0xcc, 0x8e, 0x28, 0x6, 0x26, 0x7b, 0xf9, 0x8f, 0x0, 0x80, 0x19, 0x52, 0x4a, 0x94, 0x60, 0xc6, 0x2, 0xc3, 0xc1, 0xfa, 0xe7, 0x3a, 0xef, 0x43, 0x9f, 0xed, 0x45, 0x45, 0xdc, 0x7f, 0xac, 0x4c, 0xc5, 0x8a, 0x22, 0x5c, 0x14, 0x93, 0xf, 0x68, 0x8a, 0x7d, 0xe8, 0x28, 0x7f, 0x5f, 0x29, 0xf8, 0xf1, 0xe, 0xff, 0x89, 0xd8, 0x81, 0x98, 0x8e, 0x89, 0xcc, 0x95, 0x58, 0xce, 0x3c, 0x89, 0x14, 0xd2, 0x34, 0x48, 0x25, 0x71, 0xa5, 0x3, 0xc2, 0x2a, 0xa6, 0xee, 0xee, 0x22, 0x16, 0xff, 0x8c, 0x7f, 0x8c, 0xe4, 0xa5, 0xd6, 0xf6, 0xa0, 0x99, 0x3e, 0x5d, 0x22, 0x31, 0xbb, 0x47, 0x3d, 0x8f, 0xcf, 0xe7, 0x8f, 0x1d, 0x33, 0xe8, 0x16, 0x22, 0x70, 0x53, 0x8d, 0xb5, 0x8c, 0x77, 0x12, 0x4c, 0xba, 0xc9, 0xdc, 0x3b, 0xdc, 0x45, 0xfd, 0x66, 0x4d, 0x53, 0x2e, 0x8a, 0x80, 0xb1, 0x18, 0x81, 0x3b, 0xaa, 0xc2, 0x97, 0xbf, 0xf3, 0xfe, 0x66, 0x50, 0x1b, 0xa3, 0x4b, 0xff, 0x76, 0x6c, 0x8c, 0xc0, 0x63, 0xa3, 0x44, 0xb5, 0x28, 0x9d, 0x71, 0xb2, 0x54, 0xa6, 0xc9, 0xfa, 0xa5, 0xd6, 0x9a, 0x49, 0x7a, 0xb5, 0xa0, 0xff, 0x73, 0x62, 0x0, 0x4, 0x4f, 0xb1, 0x7a, 0xcf, 0x6, 0xcd, 0x1c, 0xb6, 0x30, 0x3b, 0x1f, 0xff, 0x7c, 0x60, 0x76, 0x66, 0x7f, 0xf2, 0x5e, 0xce, 0x93, 0x70, 0x6c, 0x19, 0x9f, 0xce, 0x8d, 0x9a, 0x20, 0x3e, 0x9b, 0xab, 0x67, 0x94, 0x64, 0xd2, 0x98, 0xd8, 0xbe, 0x31, 0x2a, 0xda, 0x16, 0x47, 0xdd, 0x6b, 0x88, 0x13, 0x84, 0xb5, 0xf4, 0xc9, 0x5d, 0xb4, 0x95, 0x41, 0xa0, 0xd2};
static const uint8_t PROGMEM frame2_data[DISPLAY_BYTES] = 
{0x40, 0x2c, 0x20, 0x44, 0x54, 0x35, 0x8d, 0xa0, 0x92, 0x34, 0x3e, 0xaa, 0x80, 0x0, 0x93, 0x44, 0x9a, 0x3d, 0x1c, 0xb3, 0xc2, 0xe0, 0xe2, 0x70, 0xda, 0x6a, 0x6e, 0x35, 0xe4, 0x32, 0xc5, 0xf7, 0xd7, 0x6b, 0x90, 0xfd, 0xed, 0xa3, 0x92, 0x7c, 0xd6, 0x63, 0x7, 0x5a, 0x6e, 0x35, 0x9f, 0xe5, 0xe0, 0x49, 0xb0, 0x76, 0xdf, 0x54, 0xa5, 0xc2, 0x5, 0x2, 0x24, 0x43, 0xfb, 0xb6, 0x6b, 0x5d, 0x4c, 0xd0, 0x88, 0x4b, 0x42, 0xf0, 0xb6, 0x52, 0x1b, 0xe5, 0x68, 0x1c, 0x5e, 0xef, 0x8c, 0x25, 0x90, 0x3f, 0x2b, 0xde, 0x68, 0x8e, 0x42, 0xf8, 0x8d, 0x53, 0x90, 0xf1, 0x70, 0xa, 0x59, 0x75, 0x67, 0x12, 0x1c, 0x69, 0x34, 0x97, 0xa0, 0x80, 0x9, 0x6c, 0xb3, 0xa9, 0xd1, 0x97, 0x8c, 0xc3, 0xaa, 0x87, 0x79, 0x3b, 0x12, 0x17, 0xd7, 0xbe, 0x3d, 0xcc, 0xd6, 0x2c, 0xd, 0x92, 0x62, 0x8a, 0x50, 0x3c, 0x5, 0xee, 0xea, 0x3c, 0x9d, 0x83, 0xb1, 0x1c, 0x6f, 0xa6, 0x2b, 0xd7, 0x42, 0x58, 0xc4, 0xdc, 0x13, 0x1d, 0xb9, 0x73, 0x70, 0x5a, 0xe7, 0xee, 0x40, 0xe6, 0x96, 0x77, 0xc2, 0x4c, 0x92, 0xd5, 0x40, 0x91, 0x99, 0x8b, 0x12, 0x0, 0x95, 0x69, 0xda, 0xec, 0x65, 0x1b, 0xca, 0x55, 0x72, 0x4f, 0xad, 0x43, 0xe3, 0x0, 0x80, 0x3e, 0xb5, 0x7f, 0x70, 0xdc, 0xc0, 0xd8, 0x8d, 0xe7, 0xf5, 0x54, 0x65, 0x5b, 0x86, 0x6, 0x96, 0xaf, 0x47, 0x46, 0x34, 0x34, 0x32, 0xd6, 0xab, 0x15, 0x95, 0x84, 0x36, 0x8a, 0x2b, 0x5b, 0x95, 0x60, 0x5, 0x3d, 0xd5, 0xf7, 0x91, 0xc7, 0x11, 0x41, 0xbf, 0x91, 0x72, 0xca, 0xd4, 0x5, 0x5f, 0xee, 0x2b, 0x81, 0x92, 0x21, 0xa1, 0xd6, 0xe9, 0xd3, 0x72, 0x18, 0x13, 0xe9, 0x5f, 0x13, 0xad, 0x3f, 0x68, 0x81, 0xc8, 0x83, 0x68, 0x93, 0xb3, 0x39, 0xf, 0x97, 0x8a, 0x6b, 0x83, 0x20, 0x3c, 0xf5, 0x35, 0xc5, 0x4e, 0xb3, 0x39, 0x53, 0x2e, 0x1a, 0xfe, 0xdf, 0xd9, 0x60, 0x2f, 0xbf, 0xed, 0x20, 0x41, 0x67, 0x7a, 0xb9, 0x75, 0xbe, 0xe6, 0x9c, 0x7d, 0xd7, 0x81, 0x7b, 0x45, 0xa7, 0x96, 0x16, 0x63, 0xf9, 0x43, 0x17, 0x53, 0xa9, 0xe4, 0x82, 0xa0, 0x67, 0xe2, 0xee, 0xbc, 0x2a, 0x55, 0xa3, 0x19, 0x3, 0x80, 0x53, 0xb0, 0xe8, 0xa8, 0xe, 0xbd, 0xd1, 0x6c, 0xdf, 0xd0, 0x61, 0xe9, 0x3e, 0x6c, 0x40, 0x86, 0xec, 0x24, 0xa2, 0xe8, 0x96, 0x8f, 0x2, 0x42, 0x9b, 0xf, 0xca, 0xa2, 0x4f, 0x14, 0x43, 0xf1, 0xb0, 0xbe, 0x3a, 0x30, 0xea, 0x4f, 0x3f, 0x54, 0xcf, 0x91, 0x87, 0x4c, 0x39, 0x60, 0x3f, 0x3f, 0xd8, 0x9a, 0xf8, 0x85, 0xa4, 0x7c, 0x21, 0x9a, 0x8e, 0xb3, 0xca, 0xd9, 0x23, 0x81, 0x57, 0x2c, 0x3, 0xd3, 0x6b, 0x69, 0x3, 0x1, 0xf8, 0x7e, 0x4e, 0x5b, 0x63, 0xf1, 0x1a, 0x11, 0x9d, 0x0, 0x39, 0xc1, 0xc9, 0x75, 0xf9, 0x66, 0xcf, 0xe4, 0xae, 0xc6, 0xb9, 0x94, 0xad, 0xf9, 0x91, 0x9, 0x19, 0x2d, 0x82, 0xb, 0xdc, 0x70, 0x9, 0xca, 0x40, 0xdf, 0x23, 0x8d, 0xa6, 0xf6, 0x80, 0x38, 0xa0, 0xee, 0x13, 0xb7, 0x1b, 0xe0, 0xe4, 0x3, 0xaf, 0xff, 0x99, 0xf0, 0x21, 0x5a, 0x7c, 0x1d, 0x94, 0x37, 0x89, 0xc0, 0xd5, 0x7a, 0xd5, 0xa7, 0xc7, 0xad, 0x88, 0x40, 0x3a, 0x46, 0xb8, 0xe4, 0x13, 0xc, 0xe7, 0xb6, 0x4d, 0xe5, 0xbd, 0xa4, 0xe1, 0x0, 0x6d, 0xf7, 0xbf, 0x56, 0x13, 0xd6, 0xd9, 0xe6, 0xec, 0x6b, 0x26, 0x1b, 0x78, 0xc2, 0x8d, 0x2a, 0x2e, 0x79, 0x5, 0x69, 0x30, 0xbd, 0x9f, 0x22, 0x42, 0xaa, 0xee, 0xe8, 0x69, 0x9d, 0x66, 0x69, 0xc2, 0xc4, 0x85, 0xbc, 0xd7, 0xa2, 0x15, 0xfa, 0xc, 0x73, 0x71};
static const uint8_t PROGMEM frame3_data[DISPLAY_BYTES] = 
{0x20, 0x7b, 0x2d, 0x7f, 0x67, 0x3d, 0x8d, 0x67, 0x98, 0x19, 0x70, 0xcd, 0x92, 0x1f, 0xa, 0xc6, 0x7e, 0x93, 0x78, 0xb2, 0xf6, 0xa1, 0xe3, 0xa7, 0x27, 0x7f, 0x7, 0x3d, 0x4, 0x70, 0x55, 0x91, 0x7d, 0x1c, 0xb5, 0x4a, 0x8d, 0xa4, 0x8c, 0x78, 0xea, 0x68, 0xf, 0x76, 0x22, 0xb, 0xc9, 0x76, 0x7d, 0xed, 0x38, 0x94, 0xc8, 0x7d, 0x86, 0x2e, 0xa8, 0x17, 0x3f, 0x33, 0x3b, 0x3, 0xa, 0x23, 0x5b, 0x49, 0x2e, 0x92, 0x98, 0xbc, 0x68, 0x4b, 0x73, 0xa, 0x9a, 0x5a, 0x83, 0xfd, 0x30, 0x43, 0x40, 0x99, 0x20, 0x51, 0xdc, 0xd0, 0x18, 0x70, 0xa4, 0x3b, 0x99, 0x63, 0x18, 0xc0, 0x6c, 0x2d, 0x4c, 0xd4, 0xee, 0x2b, 0xa7, 0x74, 0x2c, 0x39, 0x81, 0x38, 0xfe, 0x26, 0xe8, 0x93, 0xf1, 0x8d, 0x9f, 0x73, 0xf2, 0x9f, 0x89, 0x4d, 0xaa, 0xbe, 0xab, 0xa0, 0x70, 0x21, 0xc2, 0x2, 0x8c, 0xb1, 0xdd, 0x20, 0x56, 0xa2, 0xe9, 0xa3, 0x27, 0x7a, 0x2b, 0x3d, 0xaf, 0xf9, 0xa5, 0x22, 0x9, 0xd5, 0x85, 0x2a, 0x1f, 0x14, 0x74, 0xb5, 0xf1, 0x3f, 0xdd, 0x46, 0xa3, 0x19, 0x3f, 0x4, 0x61, 0x70, 0xa5, 0x9, 0xfd, 0x7e, 0x85, 0xe2, 0x40, 0xf9, 0xf4, 0x1, 0xbe, 0xc4, 0x65, 0x83, 0x63, 0x53, 0xe8, 0x3c, 0xc8, 0x79, 0xb9, 0xb5, 0xb0, 0x7a, 0x88, 0xdb, 0xf8, 0x9b, 0x7a, 0x89, 0xb0, 0x2b, 0xed, 0x17, 0x3d, 0x8a, 0xa8, 0x3a, 0xa4, 0x75, 0x57, 0xd0, 0x26, 0xeb, 0x4e, 0xbd, 0x7a, 0xcc, 0xfc, 0xa0, 0xff, 0xf3, 0x7, 0x1d, 0x4d, 0xa2, 0xd1, 0x3, 0x1c, 0xf3, 0x60, 0x29, 0xbf, 0xa2, 0xdd, 0x1, 0xe9, 0x60, 0x10, 0x47, 0x5a, 0xd2, 0x9d, 0xf5, 0xf7, 0xc7, 0x72, 0xdf, 0xf5, 0x87, 0xd3, 0x19, 0x7b, 0xdb, 0x6, 0x59, 0x8e, 0xf2, 0xf1, 0x74, 0x81, 0xc9, 0x64, 0xb, 0xcb, 0xb7, 0xaf, 0x7c, 0xed, 0xc, 0xaa, 0x86, 0xab, 0xca, 0x8c, 0x9e, 0xbb, 0x81, 0x5e, 0xb4, 0xc5, 0xaa, 0x35, 0x58, 0xa, 0xa4, 0x4c, 0xf9, 0x11, 0xc9, 0x66, 0xfe, 0x65, 0xec, 0x2d, 0x1c, 0x50, 0xc1, 0xa, 0x36, 0xc6, 0xef, 0x25, 0xc9, 0xd6, 0x67, 0x7a, 0x94, 0x16, 0x58, 0x50, 0xcf, 0xb3, 0x8, 0x9c, 0xdb, 0xf1, 0x3b, 0x40, 0xc9, 0xe, 0x4a, 0xad, 0xb3, 0x8c, 0x73, 0x4b, 0x2c, 0xb0, 0x65, 0xd9, 0x9b, 0x85, 0x7a, 0xee, 0x40, 0xc4, 0xa3, 0xd, 0xc6, 0x69, 0x9a, 0x86, 0xa4, 0xdd, 0x39, 0xda, 0x76, 0x14, 0x95, 0xfe, 0xc7, 0xe0, 0x5c, 0xce, 0x5f, 0x69, 0xdc, 0x55, 0x3c, 0xfa, 0x33, 0x10, 0x13, 0xb2, 0xef, 0xd4, 0x58, 0x59, 0x18, 0x79, 0xbb, 0xea, 0x61, 0xf2, 0x75, 0x3f, 0xe0, 0x83, 0x6c, 0x5e, 0xfc, 0x35, 0xe9, 0x8f, 0x2a, 0x7e, 0x2e, 0x5d, 0x6c, 0x15, 0x2e, 0xb0, 0x54, 0x5c, 0x81, 0x31, 0xf1, 0x72, 0x4c, 0xd4, 0xa, 0x6b, 0x1b, 0x89, 0x74, 0xb5, 0x23, 0xd0, 0xc3, 0x9a, 0x39, 0xf9, 0x13, 0x88, 0x33, 0x7c, 0xbf, 0x3a, 0x8e, 0xc, 0xf0, 0xd4, 0xb9, 0xdf, 0x77, 0xc, 0x88, 0xc1, 0x65, 0xd6, 0xb4, 0xa3, 0xab, 0x61, 0x12, 0xd3, 0xd2, 0xcd, 0x6f, 0x86, 0xed, 0x20, 0xe, 0xff, 0x8b, 0x3, 0x4e, 0xb3, 0xc7, 0x7, 0x56, 0xfa, 0x62, 0xd4, 0x9, 0xc1, 0xbc, 0x8c, 0xec, 0x42, 0xab, 0x73, 0xd, 0xa4, 0x52, 0xc6, 0x44, 0x87, 0x85, 0x90, 0x2a, 0x6a, 0xf9, 0x85, 0x2, 0xa6, 0x5e, 0x3e, 0xc8, 0x2a, 0x52, 0xc5, 0xa7, 0xee, 0xf4, 0xf0, 0x27, 0xf6, 0x7d, 0x6d, 0x53, 0xcd, 0x37, 0xf2, 0xbc, 0xfa, 0x4e, 0x31, 0xe2, 0x4, 0xd4, 0x69, 0xcd, 0xce, 0xaf, 0xfe, 0xa6, 0xa2, 0x11, 0xa9, 0x3b, 0xf0, 0x77, 0x82, 0x3e, 0x3c, 0xed, 0xc6, 0x2a, 0x68, 0x31};
static const uint8_t* frames_data[] = {frame1_data, frame2_data, frame3_data}; // Contains all frames
EXTMEM uint8_t animationData[FRAME_COUNT][DISPLAY_BYTES]; // 128 x 32 display frame, with embedded frames

void setup() 
{
  // TODO: Initialize Flash and RAM
  // EXTMEM will store the big lookup tables and such in the 8MB addon PSRAM.
  // Copy these values from the SD card / Flash to the PSRAM on Init
  // 
  // Links:
  // https://www.pjrc.com/store/psram.html <- PSRAM Usage
  // https://github.com/newdigate/teensy-sample-flashloader <- Github to load MicroSD to 4.1 External Flash
  // https://github.com/PaulStoffregen/LittleFS <- Teensy LittleFS (File System)
  //
  // High-level idea:
  // Store Data in a .csv lookup table on the MicroSD card.
  // Copy this structured data from the .csv to the FLASH Memory if there are checksum differences (TODO: Checksum stuff)
  // Data verification is key. Will have to do some magic to determine data is valid and structured
  // Data size, read and compare will be utilized
  // Copy the FLASH memory to PSRAM as needed for faster access to lookup tables
  // Examples:
  // EXTMEM float phaseLookup[9999]; <- 

  //sampleTimer.begin(takeMeasurements, )
  pinMode(ADC_OUTA_CS_PIN, OUTPUT);
  SPI.begin();



  // This copies frame data
  const unsigned int frame_size = 256;
  for (int i = 0; i < FRAME_COUNT; i++)
  {
    //memcpy(animationData + FRAME_SIZE * i, &frames_data[i], FRAME_SIZE);
    memcpy(animationData[i], frames_data[i], sizeof(frames_data[i]));
  }

  // Begin Display
  /*
  if (display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS))
  {
    Serial.println("Failure of Display");
  }
  display.clearDisplay();
  int insetMargin = 1;
  display.drawRoundRect(insetMargin, insetMargin, DISPLAY_WIDTH - 2 * insetMargin, DISPLAY_HEIGHT - 2 * insetMargin, 4, SSD1306_WHITE); // Rounded Rectangle
  display.display(); // Update Contents
  */
  Serial.println("Setup successful.");
}

void loop() 
{
  // For each ADC, read the value
  for (RFInputADC selectedADC : rfADCArray)
  {
    directADCReadValue = getADCMeasure(selectedADC);
    currentSample = RFSample(selectedADC, directADCReadValue, directADCCounter);
    bigAssBuffer[directADCCounter] = currentSample;
  }
  // Do this first
  directADCReadValue = getADCMeasure(adcOutA);
  directADCCounter++; // Increment Counter
  directADCSum += directADCReadValue; // Add to rolling sum


  if (blinkTimer.check() == 1)
  {
    toggleLED();
  }
  if (serialOutputTimer.check() == 1)
  {
    directRollingAverage = directADCSum / ((float) directADCCounter); // basic.
    Serial.print("Calculated Average of ");
    Serial.print(directADCCounter);
    Serial.print(" values: ");
    Serial.print(directRollingAverage);
    Serial.print(", Voltage: ");
    Serial.println((directRollingAverage / ADC_MAX_BIT_VALUE) * 5.0);

    FILLARRAY(bigAssBuffer, SAMPLE_INVALID_VALUE);
    directADCCounter = 0;
    directADCSum = 0;
  }

/*
  if (animationTimer.check() == 1)
  {
    display.clearDisplay();

    currentFrame++;
    currentFrame %= FRAME_COUNT;
    display.drawBitmap(0, 0, frames_data[currentFrame], DISPLAY_WIDTH, DISPLAY_HEIGHT, SSD1306_WHITE);
    display.display();
    if (currentFrame == 0)
    {
      displayInverted = !displayInverted;
      display.invertDisplay(displayInverted);
    }
  }
  */
}

// Toggle Built-in LED state
void toggleLED()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

// Take measurements of the specified ADC.
// TODO: Selection measurement
uint16_t getADCMeasure(RFInputADC selectedADC)
{
  // Take selected ADC pin low to transfer data
  SPI.beginTransaction(spiADCSettings);
  digitalWrite(selectedADC.getSelectPin(), LOW);
  uint8_t biggerByte = SPI.transfer(ADC_SPI_TRANSFER_BYTE); // Idk.
  uint8_t smallerByte = SPI.transfer(ADC_SPI_TRANSFER_BYTE);
  digitalWrite(selectedADC.getSelectPin(), HIGH); // End transfer of data
  SPI.endTransaction();

  uint16_t calculatedValue; //= biggerByte + smallerByte;
  calculatedValue = (((uint16_t) biggerByte << 8 | smallerByte)); // Bit-bang math, don't stare.
  calculatedValue = calculatedValue >> 2; // 14-bit device
  return calculatedValue;
}