#include <defs.h>

class RFSample
{
    private:
        uint16_t sampleValueA;
        uint16_t sampleValueB;
        uint16_t sampleValueN;
        uint16_t sampleValueP;
        unsigned long timestamp;
    // Constructors
    public:
        RFSample(uint16_t sampleA, uint16_t sampleB, uint16_t sampleN, uint16_t sampleP, long acquireTime)
        {
            sampleValueA = sampleA;
            sampleValueB = sampleB;
            sampleValueN = sampleN;
            sampleValueP = sampleP;
            timestamp = acquireTime;
        }

        RFSample(volatile uint16_t sampleArray[ADC_NUM_CHANNELS], long acquireTime)
        {
            sampleValueA = sampleArray[adcA];
            sampleValueB = sampleArray[adcB];
            sampleValueN = sampleArray[adcN];
            sampleValueP = sampleArray[adcP];
            timestamp = acquireTime;
        }

        RFSample()
        {
            // Invalid values
            sampleValueA = RF_INVALID_VALUE;
            sampleValueB = RF_INVALID_VALUE;
            sampleValueN = RF_INVALID_VALUE;
            sampleValueP = RF_INVALID_VALUE;
            timestamp = 0L; // Invalid eventually
        }

        void setValue(EnumADC adc, float inputValue)
        {
            switch(adc)
            {
                case adcA:
                    //Serial.println("A");
                    sampleValueA = inputValue;
                    break;
                case adcB:
                    //Serial.println("B");
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

        void setAcquireTime(unsigned long time)
        {
            timestamp = time;
        }
        
        float getValue(EnumADC adc)
        {
            switch(adc)
            {
                case adcA:
                    return sampleValueA;
                case adcB:
                    return sampleValueB;
                case adcN:
                    return sampleValueN;
                case adcP:
                    return sampleValueP;
                default:
                    return RF_INVALID_VALUE;
            }
        }

        float getValueA()
        {
            return sampleValueA;
        }

        float getValueB()
        {
            return sampleValueB;
        }

        float getValueN()
        {
            return sampleValueN;
        }

        float getValueP()
        {
            return sampleValueP;
        }

        // Return Sample Values in [A, B, N, P] order as a string
        String toString()
        {
            String stringOutput = String(timestamp) + ": ";
            stringOutput += String(sampleValueA) + ", ";
            stringOutput += String(sampleValueB) + ", ";
            stringOutput += String(sampleValueN) + ", ";
            stringOutput += String(sampleValueP);
            return stringOutput;
        }
};