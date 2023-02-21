#include <Arduino.h>
#include <defs.h> // Constant Definitions

#include <Metro.h> // Task scheduler

#include <i2c_device.h> // Teensy 4 I2C Library
#include <Adafruit_GFX.h> // Graphics Lib
#include <Adafruit_SSD1306.h> // Display Lib
#include <RFInputADC.cpp> // RF Input ADC Object
#include <RFSample.cpp> // RF Sample Structure

/*-- Constants --*/
const RFInputADC adcOutA = RFInputADC(ADC_OUTA_CS_PIN, adcA);
const RFInputADC adcOutB = RFInputADC(ADC_OUTB_CS_PIN, adcB);
const RFInputADC adcOutN = RFInputADC(ADC_OUTN_CS_PIN, adcN);
const RFInputADC adcOutP = RFInputADC(ADC_OUTP_CS_PIN, adcP);
RFInputADC rfADCArray[] = {adcOutA, adcOutB, adcOutN, adcOutP};

/*-- Function Definition --*/
void toggleLED();
void takeMeasurements();
uint16_t getADCMeasure(RFInputADC selectedADC);

/*-- Constructors --*/
Metro blinkTimer = Metro(LED_BLINK_DELAY); // Idle status LED Blink
Metro serialOutputTimer = Metro(1E3); // TODO: Make this prettier.
IntervalTimer sampleTimer; 

/*-- Variables --*/
SPISettings spiADCSettings(SPI_CLK_SPEED, MSBFIRST, SPI_MODE0); // MCP33151 Maximums
uint16_t adcReadA; // Prototyping ADC value. TODO: translate into voltage
uint64_t sampleBufferSize = 0; // Counts number of measurements in a window
uint64_t adcSumA = 0;
float directRollingAverage = 0.0;

// Float works
EXTMEM RFSample adcSampleBuffer[RF_BUFFER_LENGTH]; // Buffer of Values (Float works)

RFSample currentSample = RFSample();

void setup() 
{
  Serial.println("Starting Setup");
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


  pinMode(ADC_OUTA_CS_PIN, OUTPUT); // Initialize ADC CS Pins

  SPI.begin();
  FILLARRAY(adcSampleBuffer, RFSample());


  Serial.println("Setup successful");
}

// TODO: timings for data sampling.
void loop() 
{
  currentSample = RFSample(); // set to empty value
  // For each ADC, read the value
  for (RFInputADC selectedADC : rfADCArray)
  {
    adcReadA = getADCMeasure(selectedADC);
    currentSample.setValue(selectedADC.getEnumADC(), adcReadA);
  }
  // Do this first
  adcSampleBuffer[sampleBufferSize] = currentSample;
  adcReadA = currentSample.getValue(adcA); // A Channel Sum
  sampleBufferSize++; // Increment Counter
  adcSumA += adcReadA; // Add to rolling sum


  if (blinkTimer.check() == 1)
  {
    toggleLED();
  }

  if (serialOutputTimer.check() == 1)
  {
    directRollingAverage = adcSumA / ((float) sampleBufferSize); // basic.
    // Defer basic printing data
    printChannelAverage(directRollingAverage, sampleBufferSize);

    // Get a random frame
    int selectedRandomFrame = random(sampleBufferSize); // maximum is buffer size.
    Serial.print("Big Ahh Buffer Values Frame ");
    Serial.print(selectedRandomFrame + ": ");
    Serial.println(adcSampleBuffer[selectedRandomFrame].toString());
    /*
    Serial.print(bigAssBuffer[selectedRandomFrame].getSampleValueA()); // Print all values
    Serial.print(", ");
    Serial.print(bigAssBuffer[selectedRandomFrame].getSampleValueB());
    Serial.print(", ");
    Serial.print(bigAssBuffer[selectedRandomFrame].getSampleValueN());
    Serial.print(", ");
    Serial.print(bigAssBuffer[selectedRandomFrame].getSampleValueP());
    Serial.println(", yay.");
    */

    sampleBufferSize = 0;
    adcSumA = 0;
  }
}

void adcBufferPrintFrame(RFSample selectedSample, int selectedFrame)
{
  Serial.print("Buffer Frame " + selectedFrame);
  Serial.print(": ");
  Serial.println(selectedSample.toString());
}

// Just print data. Not complex.
// TODO: input ADC enum, buffer, and sample length. get out a average
void printChannelAverage(float sampleAverage, long sampleCount)
{
  Serial.print("Calculated Average of ");
  Serial.print(sampleCount);
  Serial.print(" values: ");
  Serial.print(sampleAverage);
  Serial.print(", ");
  Serial.print((sampleAverage / ADC_MAX_BIT_VALUE) * ADC_MAX_VOLTAGE);
  Serial.println("V"); // Print unit TODO: Make this scale-aware
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
  calculatedValue = ((((uint16_t) biggerByte << 8) | smallerByte)); // Bit-bang math, don't stare.
  calculatedValue = calculatedValue >> 1; // 14-bit device, shift once?
  return calculatedValue;
}

void enterSampleIntoBuffer()
{

}