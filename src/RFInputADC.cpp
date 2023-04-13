#include <defs.h>

class RFInputADC
{
    private:
        uint8_t csp;
        uint16_t scalingFactor; // Todo: Add calibration factors  
        EnumADC adcReference;
        String adcName;
        boolean isEnabled;
        float offsetVoltage;

    // Constructors
    public:
        RFInputADC(uint16_t chipSelectPin, EnumADC reference, String channelName)
        {
            csp = chipSelectPin;
            adcReference = reference;
            adcName = channelName;
            isEnabled = true; // Default to on. Has to be turned off manually
            offsetVoltage = 0.025; // Volts
        }

        RFInputADC()
        {
            return; // WOW Hacky
        }

        // Set if channel is enabled.
        void setEnabled(boolean stateEnable)
        {
            isEnabled = stateEnable;
        }

        uint8_t getSelectPin()
        {
            return csp;
        }

        EnumADC getEnumADC()
        {
            return adcReference;
        }

        String toString()
        {
            return "ADC Channel " + adcName;
        }

        boolean getEnabled()
        {
            return isEnabled;
        }

        float getOffsetVoltage()
        {
            return offsetVoltage;
        }
};