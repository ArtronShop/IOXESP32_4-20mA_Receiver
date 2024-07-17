#ifndef __IOXESP32_4_20MA_RECEIVER_H__
#define __IOXESP32_4_20MA_RECEIVER_H__

#include <Arduino.h>
#include <Wire.h>

class Receiver4_20 {
    private:
        uint8_t _addr;
        TwoWire *_wire = NULL;
        int16_t _raw_shunt_voltage;
        int16_t _4mA_raw_value, _20mA_raw_value;

        bool writeReg(uint8_t addr, uint16_t *data) ;
        bool readReg(uint8_t addr, uint16_t *data) ;

    public:
        Receiver4_20(TwoWire *wire = &Wire, uint8_t addr = 0x45) ;
        bool begin(int16_t _4mA_raw_value = 6410, int16_t _20mA_raw_value = 31952) ;
        bool measure() ;
        int16_t raw() ;
        float current() ; // mA unit

};

#endif
