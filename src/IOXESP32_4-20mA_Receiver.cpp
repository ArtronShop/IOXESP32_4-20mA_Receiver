#include <IOXESP32_4-20mA_Receiver.h>

// Register
#define CONFIGURATION_REGISTER (0x01)
#define SHUNT_VOLTAGE_REGISTER (0x02)

typedef struct {
    uint32_t MODE : 3;    // Operating Mode
    uint32_t VSHCT : 3;   // Shunt Voltage Conversion Time
    uint32_t VBUSCT : 3;  // Bus Voltage Conversion Time
    uint32_t AVG : 3;     // Averaging Mode
    uint32_t _unused1 : 3;
    uint32_t RST : 1;  // Reset Bit
} ConfigurationRegister_t;

Receiver4_20::Receiver4_20(TwoWire* wire, uint8_t addr) : _wire(wire), _addr(addr) {
    // ---
}

bool Receiver4_20::writeReg(uint8_t addr, uint16_t* data) {
    Wire.beginTransmission(this->_addr);
    Wire.write(addr);
    Wire.write((*data >> 8) & 0xFF);
    Wire.write(*data & 0xFF);
    int error = Wire.endTransmission();
    return error == 0;
}

bool Receiver4_20::readReg(uint8_t addr, uint16_t* data) {
    Wire.beginTransmission(this->_addr);
    Wire.write(addr);
    if (this->_wire->endTransmission() != 0) {
        return false;
    }

    int n = Wire.requestFrom(this->_addr, (uint8_t)2);
    if (n != 2) {
        return false;
    }

    *data = Wire.read() << 8;
    *data |= Wire.read();

    return true;
}

bool Receiver4_20::begin(int16_t _4mA_raw_value, int16_t _20mA_raw_value,
                         int16_t _8mA_raw_value,
                         int16_t _12mA_raw_value,
                         int16_t _16mA_raw_value) {
    this->_4mA_raw_value = _4mA_raw_value;
    this->_8mA_raw_value = _8mA_raw_value;
    this->_12mA_raw_value = _12mA_raw_value;
    this->_16mA_raw_value = _16mA_raw_value;
    this->_20mA_raw_value = _20mA_raw_value;

    // Configs
    ConfigurationRegister_t configs;
    memset(&configs, 0, sizeof(configs));
    configs.MODE = 0b101;    // Shunt Voltage, Continuous
    configs.VSHCT = 0b100;   // 1.1 ms
    configs.VBUSCT = 0b100;  // 1.1 ms
    configs.AVG = 0b011;     // 64 samples
    return writeReg(0x00, (uint16_t*)&configs);
}

bool Receiver4_20::measure() {
    return readReg(1, (uint16_t*)&this->_raw_shunt_voltage);
}

int16_t Receiver4_20::raw() {
    return this->_raw_shunt_voltage;
}

float Receiver4_20::current() {
    int16_t raw = this->_raw_shunt_voltage;

    // Build active calibration points (skip INT16_MIN = not provided)
    const int16_t rawAll[5] = {
        _4mA_raw_value, _8mA_raw_value, _12mA_raw_value, _16mA_raw_value, _20mA_raw_value};
    const float mAAll[5] = {4.0f, 8.0f, 12.0f, 16.0f, 20.0f};

    int16_t rawPts[5];
    float mAPts[5];
    int n = 0;
    for (int i = 0; i < 5; i++) {
        if (rawAll[i] != INT16_MIN) {
            rawPts[n] = rawAll[i];
            mAPts[n] = mAAll[i];
            n++;
        }
    }

    // Interpolate between (raw=0, 0mA) and first calibration point
    if (raw < rawPts[0]) {
        return (float)raw / rawPts[0] * mAPts[0];
    }

    // Extrapolate above last point using last segment slope
    if (raw > rawPts[n - 1]) {
        float t = (float)(raw - rawPts[n - 2]) / (rawPts[n - 1] - rawPts[n - 2]);
        return mAPts[n - 2] + t * (mAPts[n - 1] - mAPts[n - 2]);
    }

    for (int i = 0; i < n - 1; i++) {
        if (raw <= rawPts[i + 1]) {
            float t = (float)(raw - rawPts[i]) / (rawPts[i + 1] - rawPts[i]);
            return mAPts[i] + t * (mAPts[i + 1] - mAPts[i]);
        }
    }

    return mAPts[n - 1];
}
