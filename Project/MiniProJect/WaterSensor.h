#ifndef WATER_SENSOR_H
#define WATER_SENSOR_H

#include <Arduino.h>
#include "UltrasonicSensor.h"

class WaterSensor {
private:
    // Digital sensors (removed parkDownStatus - replaced by ultrasonic)
    int parkUpStatus;
    int seaDownStatus;
    int seaUpStatus;
    int water_level_park;
    int water_level_pub;

    // Ultrasonic sensors
    UltrasonicSensor* ultrasonicPark;
    UltrasonicSensor* ultrasonicPub;

    // Thresholds for ultrasonic-based control
    float parkStartThreshold;
    float parkStopThreshold;
    float pubMinThreshold;

    // Last published values (to prevent MQTT spam)
    float lastPublishedParkPercent;
    float lastPublishedPubPercent;

public:
    WaterSensor();
    void init();
    void read();

    // Ultrasonic integration
    void initUltrasonic(UltrasonicSensor* park, UltrasonicSensor* pub);
    void readUltrasonic();
    void setThresholds(float parkStart, float parkStop, float pubMin);

    // Getter methods
    int getParkUpStatus() { return parkUpStatus; }
    int getSeaDownStatus() { return seaDownStatus; }
    int getSeaUpStatus() { return seaUpStatus; }

    // Water level checks
    bool isWaterFullInPub();
    bool shouldStopPump();
    bool shouldStartPumpUltrasonic();  // New: ultrasonic-based auto start

    // Ultrasonic getters
    float getParkWaterPercent();
    float getPubWaterPercent();

    // Publish sensor data to MQTT
    void publishLevels(bool debugMode);

    // Threshold updates (from MQTT)
    void updateParkStartThreshold(float value) { parkStartThreshold = value; }
    void updateParkStopThreshold(float value) { parkStopThreshold = value; }
    void updatePubMinThreshold(float value) { pubMinThreshold = value; }
};

#endif
