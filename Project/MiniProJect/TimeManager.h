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
    bool flag_send_set_led_today_working_pump_timer;
    bool flag_debug_SerialPrint;
    
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
    
    // Check timer
    bool checkTimerPump();
};

#endif
