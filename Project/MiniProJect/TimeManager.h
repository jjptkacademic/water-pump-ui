#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include <NTPClient.h>
#include "WaterSensor.h"

class TimeManager {
private:
    NTPClient* timeClient;
    unsigned long lastNTPUpdate;

    int dayOn_Select[7];
    long timeStart_Stop[2];
    bool flag_keep_timer_pump_working;
    bool flag_debug_SerialPrint;
    bool flag_timer_mode_enabled;  // เปิด/ปิด Timer Mode (เหมือน Auto Mode)

    // Timer execution tracking
    int lastTimerExecutedDay;  // เก็บวันที่ timer ทำงานครั้งสุดท้าย
    bool flag_timer_executed_today;  // ป้องกันการทำงานซ้ำในวันเดียวกัน
    bool last_published_executed_status;  // ป้องกัน MQTT spam

    WaterSensor* sensor;
    
    void updateNTPTime();
    int getCurrentTimeMinutes();
    int getCurrentDay();
    void debugTimerInfo(int currentDay, int currentTimeMinutes, int startTimeMinutes, int stopTimeMinutes);

public:
    TimeManager(NTPClient* client, WaterSensor* waterSensor);
    void init();
    
    // Timer settings
    void setDayOn(int day, bool enabled) { dayOn_Select[day] = enabled ? 1 : 0; }
    void setStartTime(int minutes) { timeStart_Stop[0] = minutes; }
    void setStopTime(int minutes) { timeStart_Stop[1] = minutes; }
    
    // Debug mode
    void setDebugMode(bool enabled) { flag_debug_SerialPrint = enabled; }
    bool getDebugMode() { return flag_debug_SerialPrint; }

    // Timer mode enable/disable
    void setTimerMode(bool enabled);
    bool getTimerMode() { return flag_timer_mode_enabled; }

    // Timer execution control
    void resetTimerExecutionFlag();  // Manual reset
    bool hasTimerExecutedToday() { return flag_timer_executed_today; }

    // Check timer
    bool checkTimerPump();
};

#endif
