#pragma once
#include <iostream>
#include <defs.h>

class RFInputADC
{
    private:
        uint16_t csp;
        uint16_t scalingFactor; // Todo: Add calibration factors  
        EnumADC adcReference;  

    // Constructors
    public:
        RFInputADC(uint16_t chipSelectPin, EnumADC reference)
        {
            csp = chipSelectPin;
            adcReference = reference;
        }

        RFInputADC()
        {
            return; // WOW Hacky
        }

        uint16_t getSelectPin()
        {
            return csp;
        }

        EnumADC getEnumADC()
        {
            return adcReference;
        }
};