// gesture APDS9960.h
// gesture/color/proximity sensor based on apds9960 i2c chip

#ifndef _IOTEMPOWER_APDS9960_H_
#define _IOTEMPOWER_APDS9960_H_

#include <Arduino.h>
#include <Wire.h>
#include <i2c-device.h>

// also defined in fastled
#ifdef WAIT
#undef WAIT
#endif
#include <SparkFun_APDS9960.h>

class Gesture_Apds9960 : public I2C_Device {
    private:
        SparkFun_APDS9960 *sensor = NULL;
        unsigned long last_read;
        uint8_t _threshold = 0;
        const char* _high;
        const char* _low;
    public:
        Gesture_Apds9960(const char* name);
        
        Gesture_Apds9960& with_threshold(uint8_t threshold, const char* high, const char* low) {
            _high = high;
            _low = low;
            _threshold = threshold;
            return *this;
        }

        Gesture_Apds9960& with_threshold(uint8_t threshold) {
            return with_threshold(threshold, "object", "nothing");
        }


        void i2c_start();
        bool measure();
};

#endif // _IOTEMPOWER_APDS9960_H_
