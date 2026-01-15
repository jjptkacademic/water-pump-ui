#ifndef WATER_SENSOR_H
#define WATER_SENSOR_H

#include <Arduino.h>

class WaterSensor {
private:
    int parkDownStatus;
    int parkUpStatus;
    int seaDownStatus;
    int seaUpStatus;
    int water_level_park;
    int water_level_pub;

public:
    WaterSensor();
    void init();
    void read();
    
    // Getter methods
    int getParkDownStatus() { return parkDownStatus; }
    int getParkUpStatus() { return parkUpStatus; }
    int getSeaDownStatus() { return seaDownStatus; }
    int getSeaUpStatus() { return seaUpStatus; }
    
    // Water level checks
    bool isWaterFullInPub();
    bool shouldStopPump();
    
    // Publish sensor data to MQTT
    void publishLevels(bool debugMode);
};

#endif
