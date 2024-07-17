# IOXESP32 4-20mA Receiver shield library

ไลบรารี่สำหรับ IOXESP32 4-20mA Receiver shield ใช้งานได้กับบอร์ด IOXESP32 ทุกรุ่น, Node32 Lite, NodeMCU-32S

## โค้ดโปรแกรมตัวอย่าง

```c++
#include <Arduino.h>
#include <Wire.h>
#include <IOXESP32_4-20mA_Receiver.h>

Receiver4_20 sensor(&Wire, 0x45); // หมายเลข I2C ค่าเริ่มต้นคือ 0x45 (กรณีต้องการเปลี่ยน โปรดศึกษาเอกสารการใช้งานเพิ่มเติม)

void setup() {
  Serial.begin(115200);

  Wire.begin(); // เริ่มต้นใช้งาน I2C
  while (!sensor.begin()) { // ตั้งค่า ADC
    Serial.println("Not found hardware :(");
    delay(2000);
  }
}

void loop() {
  delay(100);
  if (sensor.measure()) { // อัพเดทค่าปัจจุบันจาก ADC
    int16_t raw = sensor.raw(); // อ่านค่าข้อมูลดิบ (-32768 ถึง 32767)
    float current = sensor.current(); // อ่านค่ากระแสไฟฟ้า (หน่วย mA)
    Serial.printf("Raw=%5d   Current=%.02f mA\n", raw, current);
  } else {
    Serial.println("Hardware error !");
  }
}
```

ตัวอย่างค่าที่อ่านได้ (แสดงผลใน Serial Monitor)

```
Raw= 6405   Current=4.00 mA
Raw= 6407   Current=4.00 mA
Raw= 6406   Current=4.00 mA
Raw= 6407   Current=4.00 mA
Raw= 6408   Current=4.00 mA
Raw= 6408   Current=4.00 mA
Raw= 6407   Current=4.00 mA
```
