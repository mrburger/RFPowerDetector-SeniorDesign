#include <defs.h>
// DO I NEED THIS????
class RFSample
{
    private:
        float sampleValueA;
        float sampleValueB;
        float sampleValueN;
        float sampleValueP;
        unsigned long timestamp;
    // Constructors
    public:
        RFSample(float sampleA, float sampleB, float sampleN, float sampleP, long acquireTime)
        {
            sampleValueA = sampleA;
            sampleValueB = sampleB;
            sampleValueN = sampleN;
            sampleValueP = sampleP;
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