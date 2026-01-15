# Ultrasonic Implementation - Workaround (ไม่ใช้ MCP23017)

## สรุปแนวทาง

**ลด SensorinParkUp (D3) + ใช้ A0** → เพิ่ม Ultrasonic 2 ตัว + ก็อกน้ำ

```
Pins ที่ใช้:
- D0: ULTRASONIC_PARK_TRIG
- D8: ULTRASONIC_PARK_ECHO
- D3: ULTRASONIC_PUB_TRIG (เดิมเป็น SensorinParkUp)
- A0: ULTRASONIC_PUB_ECHO + WaterValve (shared pin - ใช้ OUTPUT สำหรับก็อก)

เหลือ: TX, RX (สำรอง)
```

---

## ไฟล์ที่ต้องสร้าง/แก้ไข

### 1️⃣ สร้างใหม่

**`UltrasonicSensor.h`** - Class definition
**`UltrasonicSensor.cpp`** - Implementation

### 2️⃣ แก้ไข

**`config.h`** - เพิ่ม pins และ calibration values
**`WaterSensor.h`** - เพิ่ม ultrasonic integration
**`WaterSensor.cpp`** - แก้ logic ใช้ ultrasonic
**`PumpController.cpp`** - แก้ shouldStopPump logic
**`MQTTHandler.cpp`** - เพิ่ม threshold topics
**`MiniProJect.ino`** - เพิ่ม ultrasonic objects

---

## Step 1: สร้าง UltrasonicSensor Class

### 📄 `UltrasonicSensor.h`

```cpp
#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

class UltrasonicSensor {
private:
    uint8_t trigPin;
    uint8_t echoPin;

    float distance_empty;    // ระยะเมื่อน้ำหมด (cm)
    float distance_full;     // ระยะเมื่อน้ำเต็ม (cm)

    float last_valid_distance;
    float current_water_level_percent;
    bool is_calibrated;
    uint8_t error_count;

    static const uint8_t MAX_ERROR_COUNT = 5;
    static const uint16_t TIMEOUT_US = 30000;

    float measureDistance();
    float calculateWaterLevel(float distance);

public:
    UltrasonicSensor(uint8_t trig, uint8_t echo);

    void init();
    void setCalibration(float empty_dist, float full_dist);

    bool read();
    float getWaterLevelPercent();
    float getDistance();

    bool isCalibrated();
    bool hasError();
    void resetError();
};

#endif
```

### 📄 `UltrasonicSensor.cpp`

```cpp
#include "UltrasonicSensor.h"

UltrasonicSensor::UltrasonicSensor(uint8_t trig, uint8_t echo) {
    trigPin = trig;
    echoPin = echo;
    distance_empty = 0.0;
    distance_full = 0.0;
    last_valid_distance = 0.0;
    current_water_level_percent = 0.0;
    is_calibrated = false;
    error_count = 0;
}

void UltrasonicSensor::init() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
}

void UltrasonicSensor::setCalibration(float empty_dist, float full_dist) {
    distance_empty = empty_dist;
    distance_full = full_dist;
    is_calibrated = (empty_dist > full_dist && full_dist > 0);
}

float UltrasonicSensor::measureDistance() {
    // ส่ง TRIG pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // อ่าน ECHO
    long duration = pulseIn(echoPin, HIGH, TIMEOUT_US);

    if (duration == 0) {
        return -1;  // timeout
    }

    // คำนวณระยะทาง (cm)
    float distance = duration * 0.034 / 2;

    // Validate range
    if (distance < 2 || distance > 400) {
        return -1;
    }

    return distance;
}

float UltrasonicSensor::calculateWaterLevel(float distance) {
    if (!is_calibrated) return -1;

    // คำนวณ %
    float level = ((distance_empty - distance) / (distance_empty - distance_full)) * 100.0;

    // จำกัด 0-100%
    if (level < 0) level = 0;
    if (level > 100) level = 100;

    return level;
}

bool UltrasonicSensor::read() {
    float distance = measureDistance();

    if (distance < 0) {
        error_count++;
        if (error_count >= MAX_ERROR_COUNT) {
            return false;  // error
        }
        // ใช้ค่าเก่า
        return true;
    }

    // Reset error
    error_count = 0;
    last_valid_distance = distance;
    current_water_level_percent = calculateWaterLevel(distance);

    return true;
}

float UltrasonicSensor::getWaterLevelPercent() {
    return current_water_level_percent;
}

float UltrasonicSensor::getDistance() {
    return last_valid_distance;
}

bool UltrasonicSensor::isCalibrated() {
    return is_calibrated;
}

bool UltrasonicSensor::hasError() {
    return error_count >= MAX_ERROR_COUNT;
}

void UltrasonicSensor::resetError() {
    error_count = 0;
}
```

---

## Step 2: แก้ config.h

```cpp
// ==================== ULTRASONIC SENSORS ====================
// Pin Definitions
#define ULTRASONIC_PARK_TRIG D0
#define ULTRASONIC_PARK_ECHO D8
#define ULTRASONIC_PUB_TRIG D3   // เดิมเป็น SensorinParkUp
#define ULTRASONIC_PUB_ECHO A0

// ลบบรรทัดนี้
// #define SensorinParkUp D3

// Calibration Values (ใส่ค่าจากการวัดจริง)
#define PARK_DISTANCE_EMPTY 150.0  // cm - เมื่อน้ำหมด
#define PARK_DISTANCE_FULL 20.0    // cm - เมื่อน้ำเต็ม
#define PUB_DISTANCE_EMPTY 200.0   // cm - เมื่อน้ำหมด
#define PUB_DISTANCE_FULL 30.0     // cm - เมื่อน้ำเต็ม

// Threshold Defaults
#define PARK_START_THRESHOLD 30.0  // เปิดปั๊มเมื่อ Park < 30%
#define PARK_STOP_THRESHOLD 90.0   // หยุดปั๊มเมื่อ Park >= 90%
#define PUB_MIN_THRESHOLD 20.0     // ต้องมีน้ำใน Pub >= 20%

// Water Valve (แสดงการไหล)
#define WaterValve D3  // หรือใช้ pin อื่นถ้ามี
```

---

## Step 3: แก้ WaterSensor Class

### 📄 `WaterSensor.h`

```cpp
// เพิ่ม include
#include "UltrasonicSensor.h"

class WaterSensor {
private:
    // Existing digital sensors
    int parkDownStatus;
    // ลบ parkUpStatus (ไม่ใช้แล้ว)
    int seaDownStatus;
    int seaUpStatus;

    // Ultrasonic sensors
    UltrasonicSensor* ultrasonicPark;
    UltrasonicSensor* ultrasonicPub;

    // Thresholds
    float parkStartThreshold;
    float parkStopThreshold;
    float pubMinThreshold;

    // Last published (ป้องกัน spam)
    float lastPublishedParkPercent;
    float lastPublishedPubPercent;

public:
    WaterSensor();
    void init();
    void read();

    // Ultrasonic
    void initUltrasonic(UltrasonicSensor* park, UltrasonicSensor* pub);
    void readUltrasonic();
    void setThresholds(float parkStart, float parkStop, float pubMin);

    // Getters
    int getParkDownStatus() { return parkDownStatus; }
    int getSeaDownStatus() { return seaDownStatus; }
    int getSeaUpStatus() { return seaUpStatus; }

    // Logic
    bool isWaterFullInPub();
    bool shouldStopPump();
    bool shouldStartPumpUltrasonic();

    // MQTT
    void publishLevels(bool debugMode);

    // Threshold updates (จาก MQTT)
    void updateParkStartThreshold(float value) { parkStartThreshold = value; }
    void updateParkStopThreshold(float value) { parkStopThreshold = value; }
    void updatePubMinThreshold(float value) { pubMinThreshold = value; }
};
```

### 📄 `WaterSensor.cpp`

```cpp
// Constructor
WaterSensor::WaterSensor() {
    parkDownStatus = 0;
    seaDownStatus = 0;
    seaUpStatus = 0;
    ultrasonicPark = nullptr;
    ultrasonicPub = nullptr;
    parkStartThreshold = 30.0;
    parkStopThreshold = 90.0;
    pubMinThreshold = 20.0;
    lastPublishedParkPercent = -999;
    lastPublishedPubPercent = -999;
}

// Init
void WaterSensor::init() {
    pinMode(SensorinSeaUp, INPUT);
    pinMode(SensorinSeaDown, INPUT);
    pinMode(SensorinParkDown, INPUT);
    // ลบ SensorinParkUp
}

void WaterSensor::initUltrasonic(UltrasonicSensor* park, UltrasonicSensor* pub) {
    ultrasonicPark = park;
    ultrasonicPub = pub;
}

void WaterSensor::setThresholds(float parkStart, float parkStop, float pubMin) {
    parkStartThreshold = parkStart;
    parkStopThreshold = parkStop;
    pubMinThreshold = pubMin;
}

// Read sensors
void WaterSensor::read() {
    parkDownStatus = digitalRead(SensorinParkDown);
    seaDownStatus = digitalRead(SensorinSeaDown);
    seaUpStatus = digitalRead(SensorinSeaUp);
}

void WaterSensor::readUltrasonic() {
    if (ultrasonicPark) ultrasonicPark->read();
    if (ultrasonicPub) ultrasonicPub->read();
}

// Logic - ใช้ Ultrasonic
bool WaterSensor::shouldStopPump() {
    // ความปลอดภัยสูงสุด: เช็คน้ำคลองหมด
    if (seaDownStatus == 1) return true;

    // ใช้ ultrasonic ตรวจน้ำสวนเต็ม
    if (ultrasonicPark && !ultrasonicPark->hasError()) {
        if (ultrasonicPark->getWaterLevelPercent() >= parkStopThreshold) {
            return true;  // น้ำเต็มแล้ว
        }
    }

    return false;
}

bool WaterSensor::shouldStartPumpUltrasonic() {
    if (!ultrasonicPark || !ultrasonicPub) return false;
    if (ultrasonicPark->hasError() || ultrasonicPub->hasError()) return false;

    float parkLevel = ultrasonicPark->getWaterLevelPercent();
    float pubLevel = ultrasonicPub->getWaterLevelPercent();

    // เริ่มปั๊มถ้า: Park น้ำน้อย AND Pub มีน้ำพอ
    return (parkLevel < parkStartThreshold) && (pubLevel > pubMinThreshold);
}

bool WaterSensor::isWaterFullInPub() {
    return (seaUpStatus == 1 && seaDownStatus == 0);
}

// Publish MQTT
void WaterSensor::publishLevels(bool debugMode) {
    if (!client.connected()) return;

    // Digital sensors (เดิม)
    // ... existing code ...

    // Ultrasonic Park
    if (ultrasonicPark && !ultrasonicPark->hasError()) {
        float parkPercent = ultrasonicPark->getWaterLevelPercent();

        // Publish เฉพาะเมื่อเปลี่ยน > 2%
        if (abs(parkPercent - lastPublishedParkPercent) > 2.0) {
            char buffer[10];
            dtostrf(parkPercent, 4, 1, buffer);
            client.publish("ptk/esp8266/ultrasonic/park/percent", buffer, true);
            lastPublishedParkPercent = parkPercent;

            if (debugMode) {
                Serial.printf("Park: %.1f%%\n", parkPercent);
            }
        }
    } else if (ultrasonicPark) {
        client.publish("ptk/esp8266/ultrasonic/park/status", "ERROR", true);
    }

    // Ultrasonic Pub
    if (ultrasonicPub && !ultrasonicPub->hasError()) {
        float pubPercent = ultrasonicPub->getWaterLevelPercent();

        if (abs(pubPercent - lastPublishedPubPercent) > 2.0) {
            char buffer[10];
            dtostrf(pubPercent, 4, 1, buffer);
            client.publish("ptk/esp8266/ultrasonic/pub/percent", buffer, true);
            lastPublishedPubPercent = pubPercent;

            if (debugMode) {
                Serial.printf("Pub: %.1f%%\n", pubPercent);
            }
        }
    } else if (ultrasonicPub) {
        client.publish("ptk/esp8266/ultrasonic/pub/status", "ERROR", true);
    }
}
```

---

## Step 4: แก้ PumpController.cpp

```cpp
// แก้ checkPumpWorking() - เพิ่มเช็ค ultrasonic
bool PumpController::checkPumpWorking() {
    // Safety first (ไม่เปลี่ยน)
    if (sensor->shouldStopPump()) {
        flag_autopump_on = false;
        return false;
    }

    // Timer mode (ไม่เปลี่ยน)
    if (flag_timer_pump) {
        return true;
    }

    // Button mode (ไม่เปลี่ยน)
    if (flag_trigBtn_start) {
        return true;
    }

    // Auto mode - ใช้ Ultrasonic
    if (sensor->shouldStartPumpUltrasonic()) {
        Serial.println("Auto start: Ultrasonic threshold");
        flag_autopump_on = true;
        return true;
    }

    // Fallback: Digital sensors
    if (sensor->getParkDownStatus() == 1 && sensor->isWaterFullInPub()) {
        Serial.println("Auto start: Digital sensors");
        flag_autopump_on = true;
        return true;
    }

    // Continue pumping
    if (flag_autopump_on) {
        return true;
    }

    return false;
}
```

---

## Step 5: เพิ่ม MQTT Topics

### 📄 `MQTTHandler.cpp`

**A. Subscribe topics (ใน reconnect()):**

```cpp
void MQTTHandler::reconnect() {
    // ... existing subscriptions ...

    // เพิ่มใหม่
    client->subscribe("ptk/esp8266/ultrasonic/threshold/park-start");
    client->subscribe("ptk/esp8266/ultrasonic/threshold/park-stop");
    client->subscribe("ptk/esp8266/ultrasonic/threshold/pub-min");
    client->subscribe("ptk/esp8266/ultrasonic/calibrate/park");
    client->subscribe("ptk/esp8266/ultrasonic/calibrate/pub");
}
```

**B. Handle topics (ใน handleCallback()):**

```cpp
void MQTTHandler::handleCallback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    String topicStr = String(topic);

    // ... existing handlers ...

    // ========== Ultrasonic Thresholds ==========
    if (topicStr == "ptk/esp8266/ultrasonic/threshold/park-start") {
        float threshold = message.toFloat();
        // ต้องเพิ่ม pointer ไปยัง WaterSensor ใน MQTTHandler
        // waterSensor->updateParkStartThreshold(threshold);
        Serial.printf("Park start: %.1f%%\n", threshold);
        client->publish("ptk/esp8266/debug", "Park start threshold updated", false);
    }
    else if (topicStr == "ptk/esp8266/ultrasonic/threshold/park-stop") {
        float threshold = message.toFloat();
        // waterSensor->updateParkStopThreshold(threshold);
        Serial.printf("Park stop: %.1f%%\n", threshold);
    }
    else if (topicStr == "ptk/esp8266/ultrasonic/threshold/pub-min") {
        float threshold = message.toFloat();
        // waterSensor->updatePubMinThreshold(threshold);
        Serial.printf("Pub min: %.1f%%\n", threshold);
    }

    // ========== Calibration (optional) ==========
    else if (topicStr == "ptk/esp8266/ultrasonic/calibrate/park") {
        int commaIndex = message.indexOf(',');
        if (commaIndex > 0) {
            float empty = message.substring(0, commaIndex).toFloat();
            float full = message.substring(commaIndex + 1).toFloat();
            // ultrasonicPark->setCalibration(empty, full);
            Serial.printf("Park calibrated: %.1f, %.1f\n", empty, full);
        }
    }
}
```

---

## Step 6: แก้ MiniProJect.ino

```cpp
// เพิ่ม include
#include "UltrasonicSensor.h"

// สร้าง global objects
UltrasonicSensor ultrasonicPark(ULTRASONIC_PARK_TRIG, ULTRASONIC_PARK_ECHO);
UltrasonicSensor ultrasonicPub(ULTRASONIC_PUB_TRIG, ULTRASONIC_PUB_ECHO);

void setup() {
    Serial.begin(115200);

    // Initialize sensors
    waterSensor.init();
    pumpController.init();

    // ========== Initialize Ultrasonic ==========
    ultrasonicPark.init();
    ultrasonicPub.init();

    // Set calibration (hardcode จากการวัด)
    ultrasonicPark.setCalibration(PARK_DISTANCE_EMPTY, PARK_DISTANCE_FULL);
    ultrasonicPub.setCalibration(PUB_DISTANCE_EMPTY, PUB_DISTANCE_FULL);

    // Link to WaterSensor
    waterSensor.initUltrasonic(&ultrasonicPark, &ultrasonicPub);
    waterSensor.setThresholds(PARK_START_THRESHOLD, PARK_STOP_THRESHOLD, PUB_MIN_THRESHOLD);

    Serial.println("Ultrasonic sensors initialized");

    // ... rest of setup ...
}

// แก้ SensorRead task
void SensorRead() {
    waterSensor.read();              // Digital sensors
    waterSensor.readUltrasonic();    // ใหม่: Ultrasonic sensors
    waterSensor.publishLevels(timeManager.getDebugMode());
}
```

---

## MQTT Topics สรุป

### ข้อมูลที่ส่งออก (ESP8266 → App):

| Topic | Payload | ความถี่ |
|-------|---------|---------|
| `ptk/esp8266/ultrasonic/park/percent` | "67.5" | เมื่อเปลี่ยน > 2% |
| `ptk/esp8266/ultrasonic/pub/percent` | "45.2" | เมื่อเปลี่ยน > 2% |
| `ptk/esp8266/ultrasonic/park/status` | "OK" / "ERROR" | เมื่อเปลี่ยนสถานะ |
| `ptk/esp8266/ultrasonic/pub/status` | "OK" / "ERROR" | เมื่อเปลี่ยนสถานะ |

### คำสั่งที่รับเข้า (App → ESP8266):

| Topic | Payload | คำอธิบาย |
|-------|---------|----------|
| `ptk/esp8266/ultrasonic/threshold/park-start` | "30" | เปิดปั๊มเมื่อ Park < 30% |
| `ptk/esp8266/ultrasonic/threshold/park-stop` | "90" | หยุดปั๊มเมื่อ Park >= 90% |
| `ptk/esp8266/ultrasonic/threshold/pub-min` | "20" | ต้องมีน้ำใน Pub >= 20% |
| `ptk/esp8266/ultrasonic/calibrate/park` | "150.5,20.3" | (Optional) Re-calibrate |
| `ptk/esp8266/ultrasonic/calibrate/pub` | "200.0,30.0" | (Optional) Re-calibrate |

**รวม: 4 topics หลัก (3 thresholds + 1 calibration per tank)**

---

## Sync กับ Timer Mode

**ไม่ต้องแก้อะไร!** ระบบ Timer จะทำงานร่วมกับ Ultrasonic อัตโนมัติ:

```cpp
// TimeManager.cpp - checkTimerPump()
// เดิมเช็ค: sensor->getSeaDownStatus() == 0 && sensor->getParkUpStatus() == 0
// ใหม่จะใช้: shouldStopPump() ซึ่งเช็ค ultrasonic อยู่แล้ว

bool TimeManager::checkTimerPump() {
    // ... existing code ...

    if (currentTimeMinutes >= startTimeMinutes &&
        currentTimeMinutes < stopTimeMinutes &&
        !sensor->shouldStopPump()) {  // ← ใช้อันเดียวกัน!

        timerActive = true;
    }

    return timerActive;
}
```

**Priority Order (ไม่เปลี่ยน):**
1. Safety check (shouldStopPump) ← ใช้ ultrasonic
2. Timer mode
3. Button mode
4. Auto mode ← ใช้ ultrasonic

---

## Calibration Process

### วิธีหาค่า Hardcode:

**1. รัน CalibrateUltrasonic.ino:**
```cpp
// บรรทัด 14-15
float distance_empty = 0.0;  // ← จดค่านี้
float distance_full = 0.0;   // ← และค่านี้
```

**2. วัดค่า:**
- เทน้ำออกจากถัง Park ให้หมด → จด distance (เช่น 150.5 cm)
- เติมน้ำถัง Park ให้เต็ม → จด distance (เช่น 20.3 cm)
- ทำซ้ำสำหรับถัง Pub

**3. ใส่ใน config.h:**
```cpp
#define PARK_DISTANCE_EMPTY 150.5
#define PARK_DISTANCE_FULL 20.3
#define PUB_DISTANCE_EMPTY 200.0
#define PUB_DISTANCE_FULL 30.0
```

---

## Testing Checklist

- [ ] Compile ผ่านไม่มี error
- [ ] Upload เข้า ESP8266
- [ ] เช็ค Serial Monitor เห็นค่า ultrasonic
- [ ] ทดสอบ MQTT publish (ดู percent topics)
- [ ] ทดสอบส่งคำสั่ง threshold จาก MQTT
- [ ] ทดสอบปั๊มเปิด-ปิดตาม threshold
- [ ] ทดสอบ fallback เมื่อ ultrasonic error
- [ ] ทดสอบ Timer mode ยังทำงานปกติ
- [ ] ทดสอบ Button mode ยังทำงานปกติ

---

## Summary

**ไฟล์ที่สร้างใหม่:** 2 ไฟล์
- `UltrasonicSensor.h`
- `UltrasonicSensor.cpp`

**ไฟล์ที่แก้ไข:** 6 ไฟล์
- `config.h` - pins + calibration
- `WaterSensor.h` - เพิ่ม ultrasonic methods
- `WaterSensor.cpp` - logic + MQTT
- `PumpController.cpp` - auto mode logic
- `MQTTHandler.cpp` - threshold topics
- `MiniProJect.ino` - init ultrasonic

**MQTT Topics เพิ่ม:** 4-5 topics
- 3 thresholds (park-start, park-stop, pub-min)
- 2 calibration (optional)

**Sync:** ใช้ logic เดิม ไม่ต้องแก้ Timer/Button modes
