# แผนการพัฒนา: เพิ่ม Ultrasonic Sensors (HC-SR04) เข้าระบบควบคุมปั๊มน้ำ

## ภาพรวม

เพิ่ม ultrasonic sensor จำนวน 2 ตัว เพื่อวัดระดับน้ำแบบต่อเนื่อง (0-100%) ในถังทั้งสอง (Park และ Pub) โดยยังคงเซนเซอร์ดิจิทัลเดิมไว้เพื่อความปลอดภัย วิธีนี้เรียกว่า "Hybrid Approach" ที่ให้ความแม่นยำในการ monitor ผ่าน MQTT พร้อมความปลอดภัยจากเซนเซอร์ดิจิทัลเป็น backup

---

## การออกแบบระบบ (Architecture)

### โครงสร้าง Class: แยก UltrasonicSensor เป็น Class ใหม่ (แนะนำ)

**Class ใหม่: UltrasonicSensor**
- จัดการเซนเซอร์ HC-SR04 ตัวเดียว (ใช้ pin TRIG และ ECHO)
- จัดการ calibration (distance_empty, distance_full)
- แปลงระยะทาง → เปอร์เซ็นต์ (0-100%)
- ตรวจจับ error และ validate ข้อมูล
- แยกออกจาก WaterSensor (ที่เป็น safety-critical)

**ปรับปรุง WaterSensor Class**
- เก็บ logic เซนเซอร์ดิจิทัลเดิมไว้ไม่เปลี่ยนแปลง
- เพิ่มการเชื่อมต่อกับ ultrasonic sensor (ใช้ pointer ไปที่ UltrasonicSensor 2 ตัว)
- จัดการ threshold configuration (เปอร์เซ็นต์ start/stop)
- ใช้ hybrid logic: shouldStartPumpUltrasonic(), shouldStopPumpUltrasonic()
- Publish ทั้งสถานะดิจิทัลและเปอร์เซ็นต์จาก ultrasonic ผ่าน MQTT

**หลักการสำคัญ:** เซนเซอร์ดิจิทัล = ผู้มีอำนาจด้านความปลอดภัย, Ultrasonic = Monitor + ควบคุมขั้นสูง

---

## Logic ควบคุมความปลอดภัยและปั๊ม

### ลำดับความสำคัญในการหยุดปั๊ม (OR logic - เงื่อนไขใดเงื่อนไขหนึ่งเป็นจริงก็หยุด):

1. **ความสำคัญสูงสุด - เซนเซอร์ดิจิทัล (ไม่เปลี่ยนแปลง):**
   - เซนเซอร์บนถัง Park ทำงาน (ป้องกันน้ำล้น)
   - เซนเซอร์ล่างถัง Pub ทำงาน (ป้องกันน้ำหมด)

2. **ความสำคัญปานกลาง - Ultrasonic Thresholds (ใหม่):**
   - ระดับน้ำ Park >= parkStopThreshold (เช่น 90%)
   - ระดับน้ำ Pub < pubMinThreshold (เช่น 20%)
   - ใช้งานได้เฉพาะเมื่อเซนเซอร์ถูก calibrate และไม่มี error

3. **พฤติกรรม Fallback:**
   - ถ้า ultrasonic เกิด error → ระบบทำงานด้วยเซนเซอร์ดิจิทัลเท่านั้น (เหมือนเดิม)
   - เซนเซอร์ดิจิทัลมีอำนาจ veto เสมอ

### เงื่อนไขการเปิดปั๊ม (AND logic - ต้องเป็นจริงทุกข้อ):

1. ผ่านการตรวจสอบความปลอดภัย (ดิจิทัล + ultrasonic ถ้ามี)
2. มี trigger การเปิด: Auto mode threshold หรือ Timer mode หรือ Manual button

---

## MQTT Topics

### ข้อมูลที่ส่งออกจาก ESP8266:
- `ptk/esp8266/ultrasonic/park/percent` - ระดับน้ำถัง Park (0-100)
- `ptk/esp8266/ultrasonic/pub/percent` - ระดับน้ำถัง Pub (0-100)
- `ptk/esp8266/ultrasonic/park/status` - สถานะ: "OK", "ERROR", "UNCALIBRATED"
- `ptk/esp8266/ultrasonic/pub/status` - สถานะ
- `ptk/esp8266/ultrasonic/park/distance` - ระยะทางดิบ (cm) สำหรับ debug
- `ptk/esp8266/ultrasonic/pub/distance` - ระยะทางดิบ (cm) สำหรับ debug

### ข้อมูลที่ ESP8266 รับจาก MQTT:
- `ptk/esp8266/ultrasonic/threshold/park-start` - Payload: "30" (%)
- `ptk/esp8266/ultrasonic/threshold/park-stop` - Payload: "90" (%)
- `ptk/esp8266/ultrasonic/threshold/pub-min` - Payload: "20" (%)
- `ptk/esp8266/ultrasonic/calibrate/park` - Payload: "150.5,20.3" (empty,full cm)
- `ptk/esp8266/ultrasonic/calibrate/pub` - Payload: "200.0,30.0" (empty,full cm)

### กลยุทธ์การ Publish:
- Publish เปอร์เซ็นต์เมื่อเปลี่ยนแปลง > 2% เท่านั้น (ป้องกัน MQTT spam)
- Publish ทุกรอบการอ่านเซนเซอร์ (200ms) แต่กรองข้อมูลซ้ำ
- ใช้ retained messages เพื่อเก็บสถานะล่าสุด

---

## ความต้องการฮาร์ดแวร์

### Pins ที่ต้องใช้:
- **รวม 4 GPIO pins** (2 เซนเซอร์ × 2 pins ต่อตัว):
  - ถัง Park: TRIG pin + ECHO pin
  - ถัง Pub: TRIG pin + ECHO pin

### ไฟเลี้ยง:
- VIN (5V) และ GND สำหรับทั้ง 2 เซนเซอร์

### การใช้ MCP23017 GPIO Expander (แนะนำ):
**ทำไมใช้ MCP23017:**
- ESP8266 pins เต็มแล้ว (D0-D8 ใช้หมด)
- MCP23017 ให้ 16 pins เพิ่ม ผ่าน I2C (ใช้แค่ 2 pins: SDA, SCL)
- รองรับแรงดัน 5V ได้ (เหมาะกับ HC-SR04)
- ราคาประมาณ 40-60 บาท

**การต่อสาย MCP23017:**
```
MCP23017          ESP8266
---------         -------
VCC      →        3.3V หรือ 5V
GND      →        GND
SDA      →        D2 (GPIO4)
SCL      →        D1 (GPIO5)
A0, A1, A2 →      GND (address 0x20)
RESET    →        3.3V
```

**Pin Assignment บน MCP23017:**
```cpp
// ใช้ไลบรารี Adafruit_MCP23017
Adafruit_MCP23017 mcp;

// Park Tank Ultrasonic
#define ULTRASONIC_PARK_TRIG 0  // GPA0 บน MCP23017
#define ULTRASONIC_PARK_ECHO 1  // GPA1 บน MCP23017

// Pub Tank Ultrasonic
#define ULTRASONIC_PUB_TRIG 2   // GPA2 บน MCP23017
#define ULTRASONIC_PUB_ECHO 3   // GPA3 บน MCP23017
```

**ไลบรารีที่ต้องติดตั้ง:**
- `Adafruit MCP23017 Arduino Library` by Adafruit
- การใช้งาน:
  ```cpp
  #include <Adafruit_MCP23017.h>

  Adafruit_MCP23017 mcp;

  void setup() {
    mcp.begin(0x20);  // I2C address
    mcp.pinMode(0, OUTPUT);  // TRIG
    mcp.pinMode(1, INPUT);   // ECHO
  }

  // ใช้แทน digitalWrite/digitalRead
  mcp.digitalWrite(0, HIGH);
  int val = mcp.digitalRead(1);
  ```

---

## ขั้นตอนการพัฒนา (Implementation)

### Phase 1: สร้าง UltrasonicSensor Class (รองรับ MCP23017)

**ไฟล์ที่ต้องสร้างใหม่:**
1. **UltrasonicSensor.h** - คำนิยาม Class
   - Constructor: `UltrasonicSensor(uint8_t trigPin, uint8_t echoPin, Adafruit_MCP23017* mcpPtr = nullptr)`
   - รองรับทั้ง ESP8266 GPIO และ MCP23017 pins
   - Methods: `init()`, `setCalibration()`, `read()`, `getWaterLevelPercent()`, `hasError()`

2. **UltrasonicSensor.cpp** - การทำงาน
   - คัดลอก logic HC-SR04 จาก CalibrateUltrasonic.ino
   - `measureDistance()` - ส่ง TRIG pulse, อ่าน ECHO, คำนวณเป็น cm
     - ใช้ `mcp->digitalWrite()` ถ้ามี MCP23017
     - ใช้ `digitalWrite()` ปกติถ้าไม่มี
   - `calculateWaterLevel()` - แปลงระยะทาง → เปอร์เซ็นต์
   - Error handling: ตรวจจับ timeout, ช่วงไม่ถูกต้อง, นับจำนวน error

**ตัวอย่างการใช้งาน:**
```cpp
// สร้าง MCP23017 object (global)
Adafruit_MCP23017 mcp;

// สร้าง UltrasonicSensor ที่ใช้ MCP23017
UltrasonicSensor ultrasonicPark(0, 1, &mcp);  // TRIG=0, ECHO=1 บน MCP
UltrasonicSensor ultrasonicPub(2, 3, &mcp);   // TRIG=2, ECHO=3 บน MCP
```

### Phase 2: ปรับปรุง WaterSensor Class

**ไฟล์ที่ต้องแก้ไข:**
1. **WaterSensor.h**
   - เพิ่ม private members: `UltrasonicSensor* ultrasonicPark/Pub`
   - เพิ่ม threshold members: `float parkStartThreshold, parkStopThreshold, pubMinThreshold`
   - เพิ่ม methods: `initUltrasonic()`, `readUltrasonic()`, `setThresholds()`, `shouldStartPumpUltrasonic()`, `shouldStopPumpUltrasonic()`, `getParkWaterPercent()`, `getPubWaterPercent()`

2. **WaterSensor.cpp**
   - เขียน `readUltrasonic()` - เรียก read() ทั้ง 2 เซนเซอร์
   - เขียน `shouldStartPumpUltrasonic()` - เช็คว่า park < startThreshold และ pub > minThreshold
   - เขียน `shouldStopPumpUltrasonic()` - เช็คว่า park >= stopThreshold หรือ pub < minThreshold
   - ปรับปรุง `publishLevels()` - Publish เปอร์เซ็นต์จาก ultrasonic พร้อมกรองแค่เปลี่ยน > 2%

### Phase 3: อัพเดท Configuration

**ไฟล์ที่ต้องแก้ไข:**
1. **config.h**
   - เพิ่มคำนิยาม MCP23017 I2C address:
     ```cpp
     #define MCP23017_ADDRESS 0x20  // I2C address
     ```
   - เพิ่มคำนิยาม pins บน MCP23017:
     ```cpp
     // Ultrasonic pins บน MCP23017
     #define ULTRASONIC_PARK_TRIG 0  // GPA0
     #define ULTRASONIC_PARK_ECHO 1  // GPA1
     #define ULTRASONIC_PUB_TRIG 2   // GPA2
     #define ULTRASONIC_PUB_ECHO 3   // GPA3
     ```
   - เพิ่มค่า calibration (จากการวัดจริง):
     ```cpp
     #define PARK_DISTANCE_EMPTY 150.0  // cm - ค่าที่วัดได้
     #define PARK_DISTANCE_FULL 20.0    // cm - ค่าที่วัดได้
     #define PUB_DISTANCE_EMPTY 200.0   // cm - ค่าที่วัดได้
     #define PUB_DISTANCE_FULL 30.0     // cm - ค่าที่วัดได้
     ```
   - เพิ่มค่า threshold เริ่มต้น:
     ```cpp
     #define PARK_START_THRESHOLD 30.0  // เปิดปั๊มที่ 30%
     #define PARK_STOP_THRESHOLD 90.0   // หยุดปั๊มที่ 90%
     #define PUB_MIN_THRESHOLD 20.0     // ต้องมีน้ำอย่างน้อย 20% ใน pub
     ```

### Phase 4: ผนวกเข้า Main Loop (รวม MCP23017)

**ไฟล์ที่ต้องแก้ไข:**
1. **MiniProJect.ino**
   - Include libraries:
     ```cpp
     #include <Adafruit_MCP23017.h>
     #include "UltrasonicSensor.h"
     ```
   - สร้าง global objects:
     ```cpp
     // MCP23017 GPIO Expander
     Adafruit_MCP23017 mcp;

     // Ultrasonic sensors ที่ใช้ MCP23017 pins
     UltrasonicSensor ultrasonicPark(ULTRASONIC_PARK_TRIG, ULTRASONIC_PARK_ECHO, &mcp);
     UltrasonicSensor ultrasonicPub(ULTRASONIC_PUB_TRIG, ULTRASONIC_PUB_ECHO, &mcp);
     ```
   - ใน `setup()` หลังจาก `setup_wifi()`:
     ```cpp
     // Initialize MCP23017
     mcp.begin(MCP23017_ADDRESS);
     Serial.println("MCP23017 initialized");

     // Initialize ultrasonic sensors
     ultrasonicPark.init();
     ultrasonicPub.init();
     ultrasonicPark.setCalibration(PARK_DISTANCE_EMPTY, PARK_DISTANCE_FULL);
     ultrasonicPub.setCalibration(PUB_DISTANCE_EMPTY, PUB_DISTANCE_FULL);
     waterSensor.initUltrasonic(&ultrasonicPark, &ultrasonicPub);
     waterSensor.setThresholds(PARK_START_THRESHOLD, PARK_STOP_THRESHOLD, PUB_MIN_THRESHOLD);
     ```
   - แก้ไข `SensorRead()` task:
     ```cpp
     void SensorRead() {
         waterSensor.read();  // เซนเซอร์ดิจิทัล
         waterSensor.readUltrasonic();  // ใหม่: Ultrasonic sensors
         waterSensor.publishLevels(timeManager.getDebugMode());
     }
     ```

### Phase 5: ปรับปรุง Pump Control

**ไฟล์ที่ต้องแก้ไข:**
1. **PumpController.cpp**
   - แก้ไข `checkPumpWorking()`:
     - หลังจากเช็คความปลอดภัยของดิจิทัล เพิ่มเช็คหยุดจาก ultrasonic
     - ใน auto mode start logic เพิ่มเช็ค ultrasonic threshold
     - รักษา flag behavior เดิมไว้
   - เซนเซอร์ดิจิทัลยังคงเป็นความสำคัญสูงสุด
   - Ultrasonic เป็นเงื่อนไขรอง stop/start

### Phase 6: MQTT Command Handler

**ไฟล์ที่ต้องแก้ไข:**
1. **MQTTHandler.h**
   - เพิ่ม method declarations สำหรับคำสั่ง ultrasonic (หรือผนวกเข้า handleCallback)
   - เพิ่ม pointer ไปยัง WaterSensor เพื่ออัพเดท threshold

2. **MQTTHandler.cpp**
   - ใน `reconnect()` เพิ่ม subscriptions สำหรับ ultrasonic topics
   - ใน `handleCallback()` เพิ่ม handlers:
     ```cpp
     else if (topicStr == "ptk/esp8266/ultrasonic/threshold/park-start") {
         float threshold = message.toFloat();
         waterSensor->updateParkStartThreshold(threshold);
     }
     // เหมือนกันสำหรับ park-stop, pub-min, calibration topics
     ```
   - Parse รูปแบบ calibration: "150.5,20.3" → แยกเป็น empty และ full

### Phase 7: Testing และ Calibration

**ก่อนเริ่มพัฒนา:**
1. **ติดตั้งไลบรารีที่จำเป็น:**
   - Arduino IDE → Tools → Manage Libraries
   - ติดตั้ง: `Adafruit MCP23017 Arduino Library` by Adafruit
   - Dependencies จะติดตั้งอัตโนมัติ (Adafruit BusIO)

2. **เชื่อมต่อฮาร์ดแวร์:**
   - ต่อ MCP23017 กับ ESP8266 ตามแผนผัง
   - ต่อ HC-SR04 ทั้ง 2 ตัว กับ MCP23017

3. **Calibration:**
   - รัน CalibrateUltrasonic.ino สำหรับถัง park (เปล่า → เต็ม)
   - บันทึกค่า distance_empty และ distance_full
   - ทำซ้ำสำหรับถัง pub
   - อัพเดท config.h ด้วยค่าที่วัดได้

**ลำดับการทดสอบ:**
1. **ทดสอบ MCP23017:**
   - ใช้ I2C Scanner เพื่อตรวจจับ address 0x20
   - ทดสอบ digitalWrite/digitalRead ทุก pin

2. **ทดสอบ Ultrasonic:**
   - Compile และ upload
   - ดู Serial Monitor สำหรับค่าจาก ultrasonic
   - ตรวจสอบว่าระยะทางถูกต้อง (cm)
   - ตรวจสอบว่าเปอร์เซ็นต์คำนวณถูก (0-100%)

3. **ทดสอบ MQTT:**
   - ตรวจสอบว่า MQTT percentage topics ถูก publish
   - ส่งคำสั่ง MQTT threshold ตรวจสอบว่าใช้งานได้

4. **ทดสอบ Pump Control:**
   - ทดสอบ auto mode กับ ultrasonic thresholds
   - ทดสอบ digital sensor safety override (ถอดสาย ultrasonic)
   - ตรวจสอบพฤติกรรม error fallback

5. **ทดสอบความเสถียร:**
   - รันทดสอบ 24 ชั่วโมง
   - ตรวจสอบ memory leaks
   - ตรวจสอบ I2C communication stability

---

## ไฟล์สำคัญที่ต้องแก้ไข

### ไฟล์ใหม่:
1. `UltrasonicSensor.h` - คำนิยาม class เซนเซอร์ ultrasonic
2. `UltrasonicSensor.cpp` - Logic การวัดของ HC-SR04 และ calibration

### ไฟล์ที่ต้องแก้ไข:
1. `WaterSensor.h` - เพิ่มการผนวก ultrasonic, thresholds
2. `WaterSensor.cpp` - เขียน hybrid logic, MQTT publishing
3. `PumpController.cpp` - ผนวกการเช็คจาก ultrasonic ใน checkPumpWorking()
4. `MQTTHandler.cpp` - เพิ่ม handlers สำหรับคำสั่ง threshold/calibration
5. `config.h` - เพิ่ม pins, ค่า calibration, threshold เริ่มต้น
6. `MiniProJect.ino` - สร้าง objects, initialize ใน setup(), แก้ SensorRead()

---

## แผนการตรวจสอบ

### การทดสอบการทำงาน:
1. ✓ Ultrasonic sensors ทั้ง 2 ตัวอ่านค่า 0-100% ได้ถูกต้อง
2. ✓ MQTT publish เปอร์เซ็นต์พร้อมกรอง noise <2%
3. ✓ การเปิด/หยุดปั๊มตาม threshold ทำงานได้
4. ✓ เซนเซอร์ดิจิทัล override การตัดสินใจของ ultrasonic ได้
5. ✓ ระบบ fallback ไปใช้ดิจิทัลเมื่อ ultrasonic error
6. ✓ คำสั่ง MQTT อัพเดท threshold ได้แบบ dynamic
7. ✓ Timer และ button modes ทำงานกับ ultrasonic safety

### การทดสอบความปลอดภัย:
1. ✓ Logic ความปลอดภัยของเซนเซอร์ดิจิทัลไม่เปลี่ยนแปลง
2. ✓ Ultrasonic errors ไม่ทำให้เกิดการทำงานที่ไม่ปลอดภัย
3. ✓ ทุก modes (auto/timer/button) ทำงานได้
4. ✓ ระบบ recover จาก sensor errors อัตโนมัติ

### การทดสอบประสิทธิภาพ:
1. ✓ Overhead การอ่านเซนเซอร์ <50ms ต่อรอบ
2. ✓ ความถี่ MQTT <1 msg/sec ต่อ topic
3. ✓ ไม่มี memory leaks หลัง 24 ชั่วโมง
4. ✓ Free heap >20KB ระหว่างการทำงาน

---

## การลดความเสี่ยง

1. **Pin conflicts:** เลือก pins อย่างระมัดระวัง หลีกเลี่ยง boot pins (GPIO0, GPIO2)
2. **Timing interference:** timeout 30ms ยอมรับได้ใน task interval 200ms
3. **Measurement noise:** กรอง 2%, ตรวจจับ error, moving average
4. **MQTT flooding:** Change threshold, rate limiting
5. **Safety regression:** เซนเซอร์ดิจิทัลยังคงเป็นผู้มีอำนาจ, ultrasonic เป็นตัวเสริมเท่านั้น

---

## ขั้นตอนถัดไป

1. ✅ ผู้ใช้สั่งซื้อ MCP23017 GPIO Expander
2. **เตรียมฮาร์ดแวร์:**
   - รอ MCP23017 มาถึง
   - เตรียม HC-SR04 จำนวน 2 ตัว
   - เตรียมสายจัมเปอร์สำหรับต่อ I2C
3. **ติดตั้งไลบรารี:**
   - Adafruit MCP23017 Arduino Library
4. **การพัฒนา:**
   - สร้าง UltrasonicSensor class (รองรับ MCP23017)
   - ปรับปรุง WaterSensor ด้วย hybrid logic
   - ผนวกเข้า main loop และ pump controller
   - เพิ่ม MQTT command handling
5. **Calibration และทดสอบ:**
   - รัน calibration process สำหรับทั้ง 2 ถัง
   - ทดสอบและ validate ทุก modes

---

## การต่อวงจรสมบูรณ์

```
HC-SR04 (Park)          MCP23017
--------------          --------
VCC        →            5V (จาก ESP8266 VIN)
TRIG       →            GPA0 (pin 0)
ECHO       →            GPA1 (pin 1)
GND        →            GND

HC-SR04 (Pub)           MCP23017
-------------           --------
VCC        →            5V (จาก ESP8266 VIN)
TRIG       →            GPA2 (pin 2)
ECHO       →            GPA3 (pin 3)
GND        →            GND

MCP23017                ESP8266
--------                -------
VCC        →            3.3V
GND        →            GND
SDA        →            D2 (GPIO4)
SCL        →            D1 (GPIO5)
A0         →            GND
A1         →            GND
A2         →            GND
RESET      →            3.3V
```

**หมายเหตุ:**
- MCP23017 ทำงานที่ 3.3V หรือ 5V ได้
- HC-SR04 ต้องใช้ 5V (ดึงจาก VIN ของ ESP8266)
- I2C pullup resistors (4.7kΩ) มักมีบนบอร์ด MCP23017 อยู่แล้ว
