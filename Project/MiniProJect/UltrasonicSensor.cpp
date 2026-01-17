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

    // อ่าน ECHO pulse พร้อม timeout
    long duration = pulseIn(echoPin, HIGH, TIMEOUT_US);

    if (duration == 0) {
        return -1;  // timeout error
    }

    // คำนวณระยะทาง (cm)
    // เสียงเดินทาง 340 m/s = 0.034 cm/μs
    // ระยะทาง = (duration / 2) * 0.034
    float distance = duration * 0.034 / 2.0;

    // Validate range (HC-SR04: 2-400 cm)
    if (distance < 2.0 || distance > 400.0) {
        return -1;  // out of range
    }

    return distance;
}

float UltrasonicSensor::calculateWaterLevel(float distance) {
    if (!is_calibrated) return -1;

    // คำนวณ % จากระยะทาง
    // น้ำเต็ม (distance_full) = 100%
    // น้ำหมด (distance_empty) = 0%
    float level = ((distance_empty - distance) / (distance_empty - distance_full)) * 100.0;

    // จำกัดค่าให้อยู่ใน 0-100%
    if (level < 0.0) level = 0.0;
    if (level > 100.0) level = 100.0;

    return level;
}

bool UltrasonicSensor::read() {
    float distance = measureDistance();

    if (distance < 0) {
        // Error occurred
        error_count++;
        if (error_count >= MAX_ERROR_COUNT) {
            // Too many errors - sensor has problem
            return false;
        }
        // Use last valid value temporarily
        return true;
    }

    // Success - reset error counter
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
