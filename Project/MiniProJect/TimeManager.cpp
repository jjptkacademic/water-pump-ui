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
    flag_timer_mode_enabled = false;  // Default: Timer mode OFF

    // Timer execution tracking
    lastTimerExecutedDay = -1;
    flag_timer_executed_today = false;
    last_published_executed_status = false;

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

    // Publish initial timer status
    if (client.connected()) {
        client.publish("ptk/esp8266/timer/executed-today", "NO", true);
        // Publish timer mode status (default: OFF)
        client.publish("ptk/esp8266/set-timer", "Timer_OFF", true);
    }
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

void TimeManager::setTimerMode(bool enabled) {
    flag_timer_mode_enabled = enabled;

    // Publish MQTT: Timer mode status
    if (client.connected()) {
        const char* status = enabled ? "Timer_ON" : "Timer_OFF";
        client.publish("ptk/esp8266/set-timer", status, true);
        Serial.printf("Timer Mode: %s\n", enabled ? "ENABLED" : "DISABLED");
    }
}

void TimeManager::resetTimerExecutionFlag() {
    flag_timer_executed_today = false;
    last_published_executed_status = false;

    // Publish MQTT: Reset flag - Timer พร้อมทำงานใหม่
    if (client.connected()) {
        client.publish("ptk/esp8266/timer/executed-today", "NO", true);
        Serial.println("Timer execution flag reset → MQTT published");
    }
}

void TimeManager::debugTimerInfo(int currentDay, int currentTimeMinutes, int startTimeMinutes, int stopTimeMinutes) {
    Serial.println("=== Timer Debug Info ===");
    Serial.printf("Timer Mode: %s\n", flag_timer_mode_enabled ? "ENABLED" : "DISABLED");
    Serial.printf("Current Day: %d (%s)\n", currentDay, dayTH[currentDay]);
    Serial.printf("Day Status: %s\n", dayOn_Select[currentDay] ? "ON" : "OFF");
    Serial.printf("Current Time: %02d:%02d\n", currentTimeMinutes / 60, currentTimeMinutes % 60);
    Serial.printf("Start Time: %02d:%02d\n", startTimeMinutes / 60, startTimeMinutes % 60);
    Serial.printf("Stop Time: %02d:%02d\n", stopTimeMinutes / 60, stopTimeMinutes % 60);
    Serial.printf("Timer Executed Today: %s\n", flag_timer_executed_today ? "YES" : "NO");
    Serial.printf("Keep Working Flag: %s\n", flag_keep_timer_pump_working ? "YES" : "NO");
    Serial.println("=====================");
}

bool TimeManager::checkTimerPump() {
    // ==================== CHECK TIMER MODE ENABLED ====================
    if (!flag_timer_mode_enabled) {
        // Timer mode ปิดอยู่ → ไม่ทำงาน
        return false;
    }

    if (!client.connected()) return false;

    int currentTimeMinutes = getCurrentTimeMinutes();
    int currentDay = getCurrentDay();
    int startTimeMinutes = timeStart_Stop[0];
    int stopTimeMinutes = timeStart_Stop[1];

    // ==================== CHECK DAY CHANGE ====================
    // ถ้าเปลี่ยนวัน → reset timer execution flag
    if (currentDay != lastTimerExecutedDay) {
        flag_timer_executed_today = false;
        lastTimerExecutedDay = currentDay;
        Serial.printf("วันใหม่: %s - Reset timer flag\n", dayTH[currentDay]);

        // Publish MQTT: Timer พร้อมทำงานใหม่
        if (client.connected()) {
            client.publish("ptk/esp8266/timer/executed-today", "NO", true);
            last_published_executed_status = false;
        }
    }

    bool timerActive = false;

    // เช็คว่าวันนี้เปิดใช้งานไหม
    if (dayOn_Select[currentDay] == 1) {
        // ==================== TIMER EXECUTION LOGIC ====================
        // เงื่อนไขการเปิดปั๊มด้วย timer:
        // 1. อยู่ในช่วงเวลาที่กำหนด
        // 2. มีน้ำในคลอง และน้ำสวนยังไม่เต็ม
        // 3. ยังไม่ได้ทำงานในวันนี้ (!flag_timer_executed_today)
        // 4. ยังไม่มี flag หยุดชั่วคราว (!flag_keep_timer_pump_working)
        if (currentTimeMinutes >= startTimeMinutes && currentTimeMinutes < stopTimeMinutes
            && sensor->getSeaDownStatus() == 0 && sensor->getParkUpStatus() == 0
            && !flag_timer_executed_today
            && !flag_keep_timer_pump_working) {

            timerActive = true;
            flag_timer_executed_today = true;  // ✅ บันทึกว่าทำงานแล้ววันนี้
            Serial.println("Timer: เปิดปั๊ม (ครั้งแรกของวัน)");

            if (!flag_send_set_led_today_working_pump_timer) {
                client.publish("ptk/esp8266/timer/today-working", "Today_ON", true);
                flag_send_set_led_today_working_pump_timer = true;
            }

            // Publish MQTT: Timer ทำงานไปแล้ววันนี้ (เฉพาะครั้งแรก)
            if (!last_published_executed_status) {
                client.publish("ptk/esp8266/timer/executed-today", "YES", true);
                last_published_executed_status = true;
            }
        }
        // ถ้าทำงานอยู่และ น้ำเต็มสวน หรือน้ำหมดคลอง ให้หยุดซะ
        else if (timerActive && sensor->shouldStopPump()) {
            Serial.println("Timer: หยุดปั๊ม (น้ำเต็มหรือน้ำคลองหมด)");
            flag_keep_timer_pump_working = true;
            timerActive = false;
        }
        // ถ้าเลยช่วงเวลาแล้ว → reset flag สำหรับรอบถัดไป
        else if (currentTimeMinutes >= stopTimeMinutes) {
            flag_keep_timer_pump_working = false;
        }
    } else {
        // วันนี้ไม่ได้เปิด timer
        if (flag_send_set_led_today_working_pump_timer) {
            flag_send_set_led_today_working_pump_timer = false;
            client.publish("ptk/esp8266/timer/today-working", "Today_OFF", true);
        }
        timerActive = false;
        flag_keep_timer_pump_working = false;
    }

    // ==================== PUBLISH STATUS ====================
    // Publish สถานะปัจจุบันถ้าเปลี่ยน (ป้องกัน MQTT spam)
    if (client.connected() && flag_timer_executed_today != last_published_executed_status) {
        const char* status = flag_timer_executed_today ? "YES" : "NO";
        client.publish("ptk/esp8266/timer/executed-today", status, true);
        last_published_executed_status = flag_timer_executed_today;
        Serial.printf("Timer executed status → %s\n", status);
    }

    if (flag_debug_SerialPrint) {
        debugTimerInfo(currentDay, currentTimeMinutes, startTimeMinutes, stopTimeMinutes);
    }

    return timerActive;
}
