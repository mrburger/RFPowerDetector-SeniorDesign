#pragma once
#include <defs.h>
#include <RFInputADC.cpp> // RF Input ADC Object
#include <RFSample.cpp> // RF Sample Structure


/*-- Function Definitions --*/
double calculateChannelSum(RFSample sampleBuffer[], long sampleCount, RFInputADC selectedADC);
double calculateChannelAverage(RFSample sampleBuffer[], long sampleCount, RFInputADC selectedADC);