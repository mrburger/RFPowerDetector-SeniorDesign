#include <defs.h>
#include <RFSample.cpp>
#include <RFInputADC.cpp>


/*-- Constants --*/

/*-- Function Definitions --*/
double calculateChannelSum(RFSample sampleBuffer[], long sampleCount, RFInputADC selectedADC);
double calculateChannelAverage(RFSample sampleBuffer[], long sampleCount, RFInputADC selectedADC);

/*-- Variables --*/

/*-- CODE --*/

// NOTES:
// Samples over time are dB scaled for AD8364.
// so a voltage is a dB measure.

// Calculate peak power frequency
// HOW: Iterate over fourier, peak will have greatest Y value


// Calculate power 
// HOW: Integrate fourier graph

// Calculate a channel sum for the samples 0 to sampleCount
double calculateChannelSum(RFSample sampleBuffer[], long sampleCount, RFInputADC selectedADC)
{
  double channelSum = 0;
  EnumADC adcEnum = selectedADC.getEnumADC(); // avoid recalculations
  for (int bufferPosition = 0; bufferPosition < sampleCount; bufferPosition++)
  {
    channelSum += sampleBuffer[bufferPosition].getValue(adcEnum);
  }
  return channelSum;
}

// Calculate an average of the selected channel
double calculateChannelAverage(RFSample sampleBuffer[], long sampleCount, RFInputADC selectedADC)
{
  double channelSum = calculateChannelSum(sampleBuffer, sampleCount, selectedADC);
  double channelAverage = channelSum / (double) sampleCount;
  return channelAverage;
}