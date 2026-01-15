#include "TimeManager.h"
#include "config.h"
#include <PubSubClient.h>

extern PubSubClient client;

const char* dayTH[] = { "จันทร์", "อังคาร", "พุธ", "พฤหัส", "ศุกร์", "เสาร์", "อาทิตย์" };

TimeManager::TimeManager(NTPClient* client, WaterSensor* waterSensor) {
    timeClient = client;
    sensor = waterSensor;
    lastNTPUpdate = 0;
    flag_keep_timer_pump_working = false;
    flag_send_set_led_today_working_pump_timer = false;
    flag_debug_SerialPrint = false;
    
    for (int i = 0; i < 7; i++) {
        dayOn_Select[i] = 0;
    }
    timeStart_Stop[0] = 0;
    timeStart_Stop[1] = 0;
}

void TimeManager::init() {
    timeClient->begin();
    timeClient->setTimeOffset(utcOffsetInSeconds);
    timeClient->update();
    lastNTPUpdate = millis();
    Serial.print("Current time: ");
    Serial.println(timeClient->getFormattedTime());
}

void TimeManager::updateNTPTime() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastNTPUpdate >= NTP_INTERVAL) {
        timeClient->update();
        lastNTPUpdate = currentMillis;
    }
}

int TimeManager::getCurrentTimeMinutes() {
    updateNTPTime();
    return timeClient->getHours() * 60 + timeClient->getMinutes();
}

int TimeManager::getCurrentDay() {
    updateNTPTime();
    int currentDay = timeClient->getDay();
    return (currentDay == 0) ? 6 : currentDay - 1;
}

void TimeManager::debugTimerInfo(int currentDay, int currentTimeMinutes, int startTimeMinutes, int stopTimeMinutes) {
    Serial.println("=== Timer Debug Info ===");
    Serial.printf("Current Day: %d (%s)\n", currentDay, dayTH[currentDay]);
    Serial.printf("Day Status: %s\n", dayOn_Select[currentDay] ? "ON" : "OFF");
    Serial.printf("Current Time: %02d:%02d\n", currentTimeMinutes / 60, currentTimeMinutes % 60);
    Serial.printf("Start Time: %02d:%02d\n", startTimeMinutes / 60, startTimeMinutes % 60);
    Serial.printf("Stop Time: %02d:%02d\n", stopTimeMinutes / 60, stopTimeMinutes % 60);
    Serial.println("=====================");
}

bool TimeManager::checkTimerPump() {
    if (!client.connected()) return false;
    
    int currentTimeMinutes = getCurrentTimeMinutes();
    int currentDay = getCurrentDay();
    int startTimeMinutes = timeStart_Stop[0];
    int stopTimeMinutes = timeStart_Stop[1];
    
    bool timerActive = false;
    
    // เช็คว่าวันนี้เปิดใช้งานไหม
    if (dayOn_Select[currentDay] == 1) {
        // เช็คว่าอยู่ในช่วงเวลาที่กำหนดไหม และในคลองมีน้ำไหม และในสวนน้ำเต็มหรือยัง
        if (currentTimeMinutes >= startTimeMinutes && currentTimeMinutes < stopTimeMinutes
            && sensor->getSeaDownStatus() == 0 && sensor->getParkUpStatus() == 0 
            && !flag_keep_timer_pump_working) {
            
            timerActive = true;
            Serial.println("On time");
            
            if (!flag_send_set_led_today_working_pump_timer) {
                client.publish("ptk/esp8266/timer/today-working", "Today_ON", true);
                flag_send_set_led_today_working_pump_timer = true;
            }
        }
        // ถ้าทำงานอยู่และ น้ำเต็มสวน หรือน้ำหมดคลอง ให้หยุดซะ
        else if (timerActive && sensor->shouldStopPump()) {
            Serial.println("Off time");
            flag_keep_timer_pump_working = true;
            timerActive = false;
        }
    } else {
        if (flag_send_set_led_today_working_pump_timer) {
            flag_send_set_led_today_working_pump_timer = false;
            client.publish("ptk/esp8266/timer/today-working", "Today_OFF", true);
        }
        timerActive = false;
        flag_keep_timer_pump_working = false;
    }
    
    if (flag_debug_SerialPrint) {
        debugTimerInfo(currentDay, currentTimeMinutes, startTimeMinutes, stopTimeMinutes);
    }
    
    return timerActive;
}
