#include <defs.h> // Constant Definitions

#include <Metro.h> // Task scheduler

#include <i2c_device.h> // Teensy 4 I2C Library
#include <SPI.h> // SPI Library for ADC
#include <Adafruit_GFX.h> // Graphics Lib
#include <Adafruit_SSD1306.h> // Display Lib
#include <RFInputADC.cpp> // RF Input ADC Object
#include <RFSample.cpp> // RF Sample Structure
#include <dataHelper.cpp> 

/*-- Constants --*/
const SPISettings spiADCSettings(SPI_CLK_SPEED, MSBFIRST, SPI_MODE0); // MCP33151 Maximums


/*-- Function Definitions --*/
void toggleLED();
void takeMeasurements();
void fastMeasure(); // Testing
uint16_t getADCMeasure(RFInputADC selectedADC);
uint16_t getADCMeasure(uint8_t adcCSPin);
void printChannelAverage(float sampleAverage, RFInputADC selectedADC);
void adcBufferPrintFrame(RFSample selectedSample, int selectedFrame);
String getFormattedString(double valueInput, String measureUnit);
float getVoltageFromADC(uint16_t adcReading);


/*-- Constructors --*/
Metro blinkTimer = Metro(LED_BLINK_DELAY); // Idle status LED Blink
Metro serialOutputTimer = Metro(1E3); // TODO: Make this prettier.
IntervalTimer rfSampleTimer; 

/*-- Variables --*/
int16_t adcReadA; // Prototyping ADC value. TODO: translate into voltage
uint16_t adcReading; // Reading generic value
volatile uint32_t sampleBufferSize = 0; // Counts number of measurements in a window
int64_t adcSumA = 0;
uint8_t biggerByte;
uint8_t smallerByte;
uint16_t calculatedValue;

float directRollingAverage = 0.0;
elapsedMicros rfSampleMicroseconds; // Counts microseconds since last buffer reset.

// Float works
EXTMEM RFSample adcSampleBuffer[RF_BUFFER_LENGTH]; // Buffer of Values (Float works)
RFSample currentRfSample = RFSample();
volatile RFSample fastSample = RFSample(); // Must be declared volatile
volatile uint16_t fastSampleArray[ADC_NUM_CHANNELS]; // Stores the values
volatile uint16_t fastSampleValue; // Stores a channel value

/*-- ADC References --*/
RFInputADC adcOutA = RFInputADC(ADC_OUTA_CS_PIN, adcA, "A");
RFInputADC adcOutB = RFInputADC(ADC_OUTB_CS_PIN, adcB, "B");
RFInputADC adcOutN = RFInputADC(ADC_OUTN_CS_PIN, adcN, "N");
RFInputADC adcOutP = RFInputADC(ADC_OUTP_CS_PIN, adcP, "P");
const std::vector<RFInputADC*> rfAdcList = {&adcOutA, &adcOutB, &adcOutN, &adcOutP}; // Vector of References

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


  pinMode(adcOutA.getSelectPin(), OUTPUT); // Initialize ADC CS Pins
  pinMode(adcOutB.getSelectPin(), OUTPUT);
  pinMode(adcOutN.getSelectPin(), OUTPUT);
  pinMode(adcOutP.getSelectPin(), OUTPUT);
  // Disable N and P Channel for now (DOESN't WORK HERE)
  // adcOutA.setEnabled(false); // Disable A Channel
  adcOutN.setEnabled(false);
  adcOutP.setEnabled(false);


  // Testing SPI1 instead
  SPI1.begin();
  FILLARRAY(adcSampleBuffer, RFSample());
  //rfSampleTimer.begin(takeMeasurements, RF_SAMPLE_INTERVAL); // small task scheduler
  rfSampleTimer.begin(fastMeasure, RF_SAMPLE_INTERVAL);

  Serial.println("Setup successful");
}

void loop() 
{
  if (blinkTimer.check() == 1)
  {
    toggleLED();
  }

  if (serialOutputTimer.check() == 1)
  {
    //directRollingAverage = adcSumA / ((float) sampleBufferSize); // basic.
    // Defer basic printing data
    //printChannelAverage(directRollingAverage, sampleBufferSize);

    // Get a random frame
    int selectedRandomFrame = random(sampleBufferSize); // maximum is buffer size.
    adcBufferPrintFrame(adcSampleBuffer[selectedRandomFrame], selectedRandomFrame);

    // Print some data
    for (RFInputADC* selectedADC : rfAdcList)
    {
      // TODO: print only onceRFInputADC* selectedADC : rfAdcList
      directRollingAverage = calculateChannelAverage(adcSampleBuffer, sampleBufferSize, *selectedADC);
      printChannelAverage(directRollingAverage, *selectedADC);
    }
    Serial.println(""); //newline

    sampleBufferSize = 0;     // Reset sample buffer size & position
    FILLARRAY(adcSampleBuffer, RFSample()); // Fill array with empty values
    adcSumA = 0;
  }
}

// Measure data a little faster.
// Based on IntervalTimer specifications instead.
// Runs as interrupt
// TODO: Test for speed
void fastMeasure()
{
   for (RFInputADC* selectedADC : rfAdcList)
  {
    fastSampleValue = 0;
    // Verify channel enabled
    if (selectedADC->getEnabled())
    {
      fastSampleValue = getADCMeasure(selectedADC->getSelectPin()); // Sped up by passing a smaller data type
    }
    fastSampleArray[selectedADC->getEnumADC()] = fastSampleValue;
  }
  if (sampleBufferSize == 0)
  {
    rfSampleMicroseconds = 0; // Reset microsecond counter
  }
  adcSampleBuffer[sampleBufferSize] = RFSample(fastSampleArray, rfSampleMicroseconds); // enter data
  sampleBufferSize++;
}

// Take measurements on all ADC Channels.
// ISSUES: SLOW! 
// TODO: Compress this into a faster function call
void takeMeasurements()
{
  currentRfSample = RFSample(); // set to empty value
  adcReading = 0; // Reset every time
  // For each enabled ADC, read the value
  
  for (RFInputADC* selectedADC : rfAdcList)
  {
    // Verify channel enabled
    if (selectedADC->getEnabled())
    {
      currentRfSample.setValue(selectedADC->getEnumADC(), getADCMeasure(*selectedADC));
    }
    /*
    else
    {
      Serial.println(selectedADC->toString() + " Disabled");
    }
    */
    //Serial.println(String(selectedADC.toString()) + ": " + String(adcReading)); // Print channel and reading
    //delay(20); //Testing to see if ADC overwhelm the BUS
  }

  if (sampleBufferSize == 0) // start of new array
  {
    rfSampleMicroseconds = 0; // Reset sample time 
  }
  currentRfSample.setAcquireTime(rfSampleMicroseconds); // Microseconds? I dont know if this will work
  //Serial.println(""); // Newline after completion

  // TODO: Buffer management. What do if the buffer length is exceeded? Clearly something needs to be done.
  // Solutions could involve waiting until the buffer is full and THEN doing all the math. Single trigger
  // Or in constant measurement, could calculate everything and write to SD card.
  // Constant measurements would have to essentially be "clocked" or time-limited, so buffer isn't exceeded.
  // For now, ignore this shit.

  adcSampleBuffer[sampleBufferSize] = currentRfSample;
  //adcReadA = currentSample.getValue(adcA); // A Channel Sum
  sampleBufferSize++; // Increment Counter
  //adcSumA += adcReadA; // Add to rolling sum
}

// Print a RFSample from the External PSRAM Buffer
void adcBufferPrintFrame(RFSample selectedSample, int selectedFrame)
{
  Serial.print("Buffer Frame " + String(selectedFrame));
  Serial.print(": ");
  Serial.println(selectedSample.toString());
  Serial.println(""); // Empty line
}



// Outputs some information on the sample buffer in question
void printSampleInformationHeader(RFSample sampleBuffer[], long sampleCount)
{
  // TODO: print time delta from last-first, sample count, etc.
  Serial.print("Samples: ");
  Serial.print(sampleCount);
  Serial.print(" Start: " + sampleBuffer[0].toString());
  Serial.println(""); // Newline

}

// Just print data. Not complex.
// TODO: input ADC enum, buffer, and sample length. get out a average
void printChannelAverage(float sampleAverage, RFInputADC selectedADC)
{
  Serial.print(selectedADC.toString() + ": ");
  Serial.print(sampleAverage);
  Serial.print(", ");
  Serial.print(getFormattedString(getVoltageFromADC(sampleAverage), VOLTAGE_SYMBOL));
  Serial.println(""); // Empty line
}

// Converts the ADC reading to a more friendly voltage.
float getVoltageFromADC(uint16_t adcReading)
{
    float voltageValue = (adcReading / ADC_MAX_BIT_VALUE) * ADC_MAX_VOLTAGE;
    return voltageValue;
}

// Formats a double-precision value with a SI unit into a pretty scaled value.
// TODO: make this also take a unit string. So it can display Amps, etc.
String getFormattedString(double valueInput, String measureUnit)
{
  // Scale value to mV or V
  String outputString = ""; // new string
  if (valueInput < 1.0) // Below 1V, we are in millivolt scale
  {
    valueInput *= MILLI_SCALE_FACTOR; // Scale by 1000 due to millivolt
    outputString = String(round(valueInput)) + MILLI_PREFIX_SYMBOL + measureUnit; // TODO: make this more modular. maybe format better
  }
  else
  {
    // Above 1.0V, round to 2 decimal places I guess
    int decimalScalingValue = pow(10, MEASUREMENT_DECIMALS);
    valueInput *= decimalScalingValue; // Multiply by 100, this will give us 2 decimal places.
    valueInput = round(valueInput) / (float) decimalScalingValue; // Reduce it back down to normal scale.
    outputString = String(valueInput) + measureUnit;
  }
  return outputString;
}


// Toggle Built-in LED state
// DO NOT USE WITH SPI
void toggleLED()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

uint16_t getADCMeasure(u_int8_t adcCSPin)
{
  SPI1.beginTransaction(spiADCSettings);
  // Take selected ADC pin low to transfer data
  digitalWrite(adcCSPin, LOW);
  biggerByte = SPI1.transfer(ADC_SPI_TRANSFER_BYTE); // Transfer 255, 8-bit maximum
  smallerByte = SPI1.transfer(ADC_SPI_TRANSFER_BYTE);
  digitalWrite(adcCSPin, HIGH); // End transfer of data
  SPI1.endTransaction();

  //((((uint16_t) biggerByte) << 8 | smallerByte)); // Bit-bang math, don't stare.
  calculatedValue = biggerByte;
  calculatedValue << 8;
  calculatedValue |= smallerByte;
  calculatedValue >> 2; // 14-bit device, shift once? or twice? Datasheet says twice
  return calculatedValue;
}

// Take measurements of the specified ADC.
uint16_t getADCMeasure(RFInputADC selectedADC)
{
  uint16_t outputValue = getADCMeasure(selectedADC.getSelectPin());
  //Serial.print(selectedADC.toString());
  //Serial.println("Val: " + String(outputValue));
  return outputValue;
}