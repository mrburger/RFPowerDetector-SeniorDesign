#pragma once
#include <iostream>
//#include <RFInputADC.cpp>
#include <defs.h>

// DO I NEED THIS????
class RFSample
{
    private:
        float sampleValueA;
        float sampleValueB;
        float sampleValueN;
        float sampleValueP;
    // Constructors
    public:
        RFSample(float sampleA, float sampleB, float sampleN, float sampleP)
        {
            
        }

        RFSample()
        {
            
        }

        void setValue(EnumADC adc, float inputValue)
        {
            switch(adc)
            {
                case adcA:
                    sampleValueA = inputValue;
                    break;
                case adcB:
                    sampleValueB = inputValue;
                    break;
                case adcN:
                    sampleValueN = inputValue;
                    break;
                case adcP:
                    sampleValueP = inputValue;
                    break;
                default:
                    break; // Ignore
            }
        }

        float getSampleValueA()
        {
            return sampleValueA;
        }
};