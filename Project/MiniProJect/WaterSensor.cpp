#include "WaterSensor.h"
#include "config.h"
#include <PubSubClient.h>

extern PubSubClient client;

WaterSensor::WaterSensor() {
    parkDownStatus = 0;
    parkUpStatus = 0;
    seaDownStatus = 0;
    seaUpStatus = 0;
    water_level_park = 0;
    water_level_pub = 0;
}

void WaterSensor::init() {
    pinMode(SensorinSeaUp, INPUT);
    pinMode(SensorinSeaDown, INPUT);
    pinMode(SensorinParkUp, INPUT);
    pinMode(SensorinParkDown, INPUT);
}

void WaterSensor::read() {
    parkDownStatus = digitalRead(SensorinParkDown);
    parkUpStatus = digitalRead(SensorinParkUp);
    seaDownStatus = digitalRead(SensorinSeaDown);
    seaUpStatus = digitalRead(SensorinSeaUp);
}

bool WaterSensor::isWaterFullInPub() {
    return (seaUpStatus == 1 && seaDownStatus == 0);
}

bool WaterSensor::shouldStopPump() {
    return (seaDownStatus == 1 || parkUpStatus == 1);
}

void WaterSensor::publishLevels(bool debugMode) {
    if (!client.connected()) return;
    
    // Publish water level in canal (pub)
    if (seaUpStatus == 1) {
        if (water_level_pub != 3) {
            client.publish("ptk/esp8266/water-level-pub", "2", true);
            if (debugMode) Serial.println("น้ำเต็มคลอง");
        }
        water_level_pub = 3;
    } else if (seaDownStatus == 0) {
        if (water_level_pub != 2) {
            client.publish("ptk/esp8266/water-level-pub", "1", true);
            if (debugMode) Serial.println("น้ำครึ่งคลอง");
        }
        water_level_pub = 2;
    } else if (seaDownStatus == 1) {
        if (water_level_pub != 1) {
            client.publish("ptk/esp8266/water-level-pub", "0", true);
            if (debugMode) Serial.println("น้ำหมดคลองแล้ว");
        }
        water_level_pub = 1;
    }
    
    // Publish water level in park
    if (parkUpStatus == 1) {
        if (water_level_park != 3) {
            client.publish("ptk/esp8266/water-level-park", "2", true);
            if (debugMode) Serial.println("น้ำเต็มสวน");
        }
        water_level_park = 3;
    } else if (parkDownStatus == 0) {
        if (water_level_park != 2) {
            client.publish("ptk/esp8266/water-level-park", "1", true);
            if (debugMode) Serial.println("น้ำครึ่งสวน");
        }
        water_level_park = 2;
    } else if (parkDownStatus == 1) {
        if (water_level_park != 1) {
            client.publish("ptk/esp8266/water-level-park", "0", true);
            if (debugMode) Serial.println("น้ำหมดสวนแล้ว");
        }
        water_level_park = 1;
    }
}
