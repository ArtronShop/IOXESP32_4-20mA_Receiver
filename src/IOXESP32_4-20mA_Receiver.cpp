#include <IOXESP32_4-20mA_Receiver.h>

// Register
#define CONFIGURATION_REGISTER (0x01)
#define SHUNT_VOLTAGE_REGISTER (0x02)


typedef struct {
  uint32_t MODE : 3; // Operating Mode
  uint32_t VSHCT : 3; // Shunt Voltage Conversion Time
  uint32_t VBUSCT : 3; // Bus Voltage Conversion Time
  uint32_t AVG : 3; // Averaging Mode
  uint32_t _unused1 : 3;
  uint32_t RST : 1; // Reset Bit
} ConfigurationRegister_t;


Receiver4_20::Receiver4_20(TwoWire *wire, uint8_t addr) : _wire(wire), _addr(addr) {
  // ---
}

bool Receiver4_20::writeReg(uint8_t addr, uint16_t *data) {
  Wire.beginTransmission(this->_addr);
  Wire.write(addr);
  Wire.write((*data >> 16) & 0xFF);
  Wire.write(*data & 0xFF);
  int error = Wire.endTransmission();
  return error == 0;
}

bool Receiver4_20::readReg(uint8_t addr, uint16_t *data) {
  Wire.beginTransmission(this->_addr);
  Wire.write(addr);
  if (this->_wire->endTransmission() != 0) {
    return false;
  }

  int n = Wire.requestFrom(this->_addr, (uint8_t) 2);
  if (n != 2) {
    return false;
  }

  *data = Wire.read() << 8;
  *data |= Wire.read();

  return true;
}

bool Receiver4_20::begin(int16_t _4mA_raw_value, int16_t _20mA_raw_value) {
  this->_4mA_raw_value = _4mA_raw_value;
  this->_20mA_raw_value = _20mA_raw_value;

  // Configs
  ConfigurationRegister_t configs;
  memset(&configs, 0, sizeof(configs));
  configs.MODE = 0b101; // Shunt Voltage, Continuous
  configs.VSHCT = 0b100; // 1.1 ms
  configs.VBUSCT = 0b100; // 1.1 ms
  configs.AVG = 0b000; // 1
  return writeReg(0x00, (uint16_t*) &configs);
}

bool Receiver4_20::measure() {
  return readReg(1, (uint16_t *) &this->_raw_shunt_voltage);
}

int16_t Receiver4_20::raw() {
  return this->_raw_shunt_voltage;
}

float Receiver4_20::current() {
  return map(this->_raw_shunt_voltage, this->_4mA_raw_value, this->_20mA_raw_value, 400, 2000) / 100.0f;
}

