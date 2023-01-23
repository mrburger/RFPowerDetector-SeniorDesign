#pragma once
#include <iostream>

class RFInputADC
{
    private:
        uint16_t csp;
        uint16_t scalingFactor; // Todo: Add calibration factors    

    // Constructors
    public:
        RFInputADC(uint16_t chipSelectPin)
        {
            csp = chipSelectPin;
        }

        uint16_t getSelectPin()
        {
            return csp;
        }
};