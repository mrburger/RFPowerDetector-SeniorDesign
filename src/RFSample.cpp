#pragma once
#include <iostream>
#include <RFInputADC.cpp>

class RFSample
{
    private:
       float sampleValue;
       long counter;
       RFInputADC parentADC;

    // Constructors
    public:
        RFSample(RFInputADC adc, float value, long counterValue)
        {
            sampleValue = value;
            counter = counterValue;
            parentADC = adc;
        }

        float getSampleValue()
        {
            return sampleValue;
        }
};