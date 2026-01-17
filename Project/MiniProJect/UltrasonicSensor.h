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
    static const uint16_t TIMEOUT_US = 30000;  // 30ms timeout

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
