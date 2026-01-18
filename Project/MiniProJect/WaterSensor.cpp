#include "WaterSensor.h"
#include "config.h"
#include <PubSubClient.h>

extern PubSubClient client;

WaterSensor::WaterSensor() {
    parkUpStatus = 0;
    seaDownStatus = 0;
    seaUpStatus = 0;
    water_level_park = 0;
    water_level_pub = 0;

    // Ultrasonic initialization
    ultrasonicPark = nullptr;
    ultrasonicPub = nullptr;
    parkStartThreshold = 30.0;
    parkStopThreshold = 90.0;
    pubMinThreshold = 20.0;
    lastPublishedParkPercent = -999.0;
    lastPublishedPubPercent = -999.0;
}

void WaterSensor::init() {
    // User requested INPUT_PULLUP for these pins
    pinMode(SensorinSeaUp, INPUT_PULLUP);
    pinMode(SensorinSeaDown, INPUT_PULLUP);
    pinMode(SensorinParkUp, INPUT_PULLUP);
}

void WaterSensor::read() {
    parkUpStatus = digitalRead(SensorinParkUp);
    seaDownStatus = digitalRead(SensorinSeaDown);
    seaUpStatus = digitalRead(SensorinSeaUp);
}

bool WaterSensor::isWaterFullInPub() {
    // SeaUp: 1=Empty, 0=Water
    // SeaDown: 0=Empty, 1=Water
    // Full = SeaUp has Water (0) AND SeaDown has Water (1)
    return (seaUpStatus == 0 && seaDownStatus == 1);
}

// ==================== ULTRASONIC METHODS ====================

void WaterSensor::initUltrasonic(UltrasonicSensor* park, UltrasonicSensor* pub) {
    ultrasonicPark = park;
    ultrasonicPub = pub;
}

void WaterSensor::readUltrasonic() {
    if (ultrasonicPark) ultrasonicPark->read();
    if (ultrasonicPub) ultrasonicPub->read();
}

void WaterSensor::setThresholds(float parkStart, float parkStop, float pubMin) {
    parkStartThreshold = parkStart;
    parkStopThreshold = parkStop;
    pubMinThreshold = pubMin;
}

bool WaterSensor::shouldStartPumpUltrasonic() {
    // Need both sensors and must be calibrated
    if (!ultrasonicPark || !ultrasonicPub) return false;
    if (ultrasonicPark->hasError() || ultrasonicPub->hasError()) return false;
    if (!ultrasonicPark->isCalibrated() || !ultrasonicPub->isCalibrated()) return false;

    float parkLevel = ultrasonicPark->getWaterLevelPercent();
    float pubLevel = ultrasonicPub->getWaterLevelPercent();

    // Start pump if: Park water low AND Pub water sufficient
    return (parkLevel < parkStartThreshold) && (pubLevel > pubMinThreshold);
}

float WaterSensor::getParkWaterPercent() {
    if (ultrasonicPark && !ultrasonicPark->hasError()) {
        return ultrasonicPark->getWaterLevelPercent();
    }
    return -1.0;  // Error or not available
}

float WaterSensor::getPubWaterPercent() {
    if (ultrasonicPub && !ultrasonicPub->hasError()) {
        return ultrasonicPub->getWaterLevelPercent();
    }
    return -1.0;  // Error or not available
}

bool WaterSensor::shouldStopPump() {
    // Priority 1: Digital sensor - น้ำคลองหมด (safety critical!)
    // SeaDown: 0 = Empty (No water)
    if (seaDownStatus == 0) {
        return true;  // Stop immediately - no water in pub/sea
    }

    // Priority 2: Digital sensor - น้ำสวนเต็ม (backup safety)
    // ParkUp: 1 = Empty, 0 = Full (Water present)
    if (parkUpStatus == 0) {
        return true;  // Stop immediately - park tank full
    }

    // Priority 3: Ultrasonic - น้ำสวนเต็ม (stop before reaching parkUp sensor)
    if (ultrasonicPark && !ultrasonicPark->hasError()) {
        if (ultrasonicPark->getWaterLevelPercent() >= parkStopThreshold) {
            return true;  // Stop at threshold (default 90%)
        }
    }

    // Priority 4: Ultrasonic - น้ำคลองต่ำ (stop before running out)
    if (ultrasonicPub && !ultrasonicPub->hasError()) {
        if (ultrasonicPub->getWaterLevelPercent() < pubMinThreshold) {
            return true;  // Stop - pub water too low (default 20%)
        }
    }

    return false;
}

void WaterSensor::publishLevels(bool debugMode) {
    if (!client.connected()) return;

    // ==================== DIGITAL SENSORS (Pub/Sea) ====================
    // SeaUp: 1=Empty, 0=Water
    // SeaDown: 0=Empty, 1=Water

    if (seaUpStatus == 0) { // Has water at top
        if (water_level_pub != 3) {
            client.publish("ptk/esp8266/water-level-pub", "2", true);
            if (debugMode) Serial.println("น้ำเต็มคลอง");
        }
        water_level_pub = 3;
    } else if (seaDownStatus == 1) { // Has water at bottom (but top is empty because else if)
        if (water_level_pub != 2) {
            client.publish("ptk/esp8266/water-level-pub", "1", true);
            if (debugMode) Serial.println("น้ำครึ่งคลอง");
        }
        water_level_pub = 2;
    } else { // SeaDown == 0 (Empty)
        if (water_level_pub != 1) {
            client.publish("ptk/esp8266/water-level-pub", "0", true);
            if (debugMode) Serial.println("น้ำหมดคลองแล้ว");
        }
        water_level_pub = 1;
    }

    // ==================== DIGITAL SENSORS (Park) ====================
    // ParkUp: 1=Empty, 0=Water (Full)

    if (parkUpStatus == 0) { // Full
        if (water_level_park != 3) {
            client.publish("ptk/esp8266/water-level-park", "2", true);
            if (debugMode) Serial.println("น้ำเต็มสวน");
        }
        water_level_park = 3;
    } else {
        // Use ultrasonic for park level if available
        if (ultrasonicPark && !ultrasonicPark->hasError()) {
            float percent = ultrasonicPark->getWaterLevelPercent();
            if (percent < 50.0) {
                if (water_level_park != 1) {
                    client.publish("ptk/esp8266/water-level-park", "0", true);
                    if (debugMode) Serial.println("น้ำต่ำสวน (ultrasonic)");
                }
                water_level_park = 1;
            } else {
                if (water_level_park != 2) {
                    client.publish("ptk/esp8266/water-level-park", "1", true);
                    if (debugMode) Serial.println("น้ำครึ่งสวน (ultrasonic)");
                }
                water_level_park = 2;
            }
        }
    }

    // ==================== ULTRASONIC SENSORS ====================

    // Publish Park ultrasonic percentage
    if (ultrasonicPark && !ultrasonicPark->hasError() && ultrasonicPark->isCalibrated()) {
        float parkPercent = ultrasonicPark->getWaterLevelPercent();

        // Only publish if changed > 2% (prevent MQTT spam)
        if (abs(parkPercent - lastPublishedParkPercent) > 2.0) {
            char buffer[10];
            dtostrf(parkPercent, 4, 1, buffer);
            client.publish("ptk/esp8266/ultrasonic/park/percent", buffer, true);
            lastPublishedParkPercent = parkPercent;

            if (debugMode) {
                Serial.print("Park ultrasonic: ");
                Serial.print(parkPercent);
                Serial.println("%");
            }
        }

        // Publish OK status
        client.publish("ptk/esp8266/ultrasonic/park/status", "OK", true);
    } else if (ultrasonicPark) {
        // Publish error status
        client.publish("ptk/esp8266/ultrasonic/park/status", "ERROR", true);
    }

    // Publish Pub ultrasonic percentage
    if (ultrasonicPub && !ultrasonicPub->hasError() && ultrasonicPub->isCalibrated()) {
        float pubPercent = ultrasonicPub->getWaterLevelPercent();

        // Only publish if changed > 2%
        if (abs(pubPercent - lastPublishedPubPercent) > 2.0) {
            char buffer[10];
            dtostrf(pubPercent, 4, 1, buffer);
            client.publish("ptk/esp8266/ultrasonic/pub/percent", buffer, true);
            lastPublishedPubPercent = pubPercent;

            if (debugMode) {
                Serial.print("Pub ultrasonic: ");
                Serial.print(pubPercent);
                Serial.println("%");
            }
        }

        // Publish OK status
        client.publish("ptk/esp8266/ultrasonic/pub/status", "OK", true);
    } else if (ultrasonicPub) {
        // Publish error status
        client.publish("ptk/esp8266/ultrasonic/pub/status", "ERROR", true);
    }
}
