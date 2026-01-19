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

    // Force NTP update หลายครั้งเพื่อให้แน่ใจว่า sync
    Serial.println("Syncing NTP time...");
    for (int i = 0; i < 5; i++) {
        if (timeClient->update()) {
            Serial.printf("✅ NTP Sync Success (attempt %d)\n", i + 1);
            break;
        }
        Serial.printf("⚠️ NTP Sync attempt %d failed, retrying...\n", i + 1);
        delay(1000);
    }

    lastNTPUpdate = millis();
    Serial.printf("Current time: %s (UTC+7 offset: %ld seconds)\n",
                  timeClient->getFormattedTime().c_str(),
                  utcOffsetInSeconds);
    Serial.printf("Epoch time: %lu\n", timeClient->getEpochTime());
    Serial.printf("Day of week: %d\n", timeClient->getDay());

    // Publish initial timer status
    if (client.connected()) {
        client.publish("ptk/esp8266/timer/executed-today", "NO", true);
        // Publish timer mode status (default: OFF) ไปที่ status topic
        client.publish("ptk/esp8266/timer/status", "Timer_OFF", true);
    }
}

void TimeManager::updateNTPTime() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastNTPUpdate >= NTP_INTERVAL) {
        bool success = timeClient->update();
        lastNTPUpdate = currentMillis;
        if (success) {
            Serial.printf("✅ NTP Updated: %s (Epoch: %lu)\n",
                          timeClient->getFormattedTime().c_str(),
                          timeClient->getEpochTime());
        } else {
            Serial.println("❌ NTP Update Failed!");
        }
    }
}

int TimeManager::getCurrentTimeMinutes() {
    updateNTPTime();
    int hours = timeClient->getHours();
    int minutes = timeClient->getMinutes();

    static unsigned long lastTimeLog = 0;
    if (millis() - lastTimeLog > 30000) {  // Log ทุก 30 วินาที
        Serial.printf("🕐 NTP Time: %02d:%02d (FormattedTime: %s)\n",
                      hours, minutes, timeClient->getFormattedTime().c_str());
        lastTimeLog = millis();
    }

    return hours * 60 + minutes;
}

int TimeManager::getCurrentDay() {
    updateNTPTime();
    int currentDay = timeClient->getDay();
    int adjustedDay = (currentDay == 0) ? 6 : currentDay - 1;

    static unsigned long lastDayLog = 0;
    if (millis() - lastDayLog > 30000) {  // Log ทุก 30 วินาที
        Serial.printf("📅 Day: NTP=%d → Adjusted=%d (%s)\n",
                      currentDay, adjustedDay, dayTH[adjustedDay]);
        lastDayLog = millis();
    }

    return adjustedDay;
}

void TimeManager::setTimerMode(bool enabled) {
    // เช็คว่า state เปลี่ยนหรือเปล่า (ป้องกัน feedback loop)
    if (flag_timer_mode_enabled == enabled) {
        return;  // ไม่เปลี่ยน → ไม่ต้องทำอะไร
    }

    flag_timer_mode_enabled = enabled;

    // Publish MQTT: Timer mode status (publish ไป topic คนละตัวเพื่อป้องกัน feedback loop)
    if (client.connected()) {
        const char* status = enabled ? "Timer_ON" : "Timer_OFF";
        client.publish("ptk/esp8266/timer/status", status, true);  // ✅ เปลี่ยน topic
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
        static unsigned long lastModeDisabledLog = 0;
        if (millis() - lastModeDisabledLog > 10000) {  // Log ทุก 10 วินาที
            Serial.println("⚠️ Timer Mode: DISABLED");
            lastModeDisabledLog = millis();
        }
        return false;
    }

    if (!client.connected()) return false;

    int currentTimeMinutes = getCurrentTimeMinutes();
    int currentDay = getCurrentDay();
    int startTimeMinutes = timeStart_Stop[0];
    int stopTimeMinutes = timeStart_Stop[1];

    // ==================== DEBUG: ALWAYS LOG TIMER CHECK ====================
    static unsigned long lastDebugLog = 0;
    if (millis() - lastDebugLog > 5000) {  // Log ทุก 5 วินาที
        unsigned long epoch = timeClient->getEpochTime();
        bool timeValid = (epoch > 1577836800);  // > 1 Jan 2020

        Serial.printf("[Timer Check] Day:%d(%s) DayEnabled:%d Time:%02d:%02d Start:%02d:%02d Stop:%02d:%02d %s\n",
                      currentDay, dayTH[currentDay], dayOn_Select[currentDay],
                      currentTimeMinutes/60, currentTimeMinutes%60,
                      startTimeMinutes/60, startTimeMinutes%60,
                      stopTimeMinutes/60, stopTimeMinutes%60,
                      timeValid ? "✅" : "❌ TIME NOT SYNCED!");
        lastDebugLog = millis();
    }

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
        // อยู่ในช่วงเวลาที่กำหนด
        if (currentTimeMinutes >= startTimeMinutes && currentTimeMinutes < stopTimeMinutes) {

            // เงื่อนไขการทำงาน: มีน้ำในคลอง และน้ำสวนยังไม่เต็ม
            // SeaDown: 1=มีน้ำ, 0=หมด | ParkUp: 1=ว่าง, 0=เต็ม
            bool shouldPumpRun = (sensor->getSeaDownStatus() == 1 && sensor->getParkUpStatus() == 1);

            // ยังไม่เคยทำงานวันนี้ และยังไม่ถูกหยุดชั่วคราว → เริ่มทำงานครั้งแรก
            if (!flag_timer_executed_today && !flag_keep_timer_pump_working) {
                if (shouldPumpRun) {
                    timerActive = true;
                    flag_timer_executed_today = true;  // ✅ บันทึกว่าเริ่มทำงานแล้ววันนี้
                    Serial.println("✅ Timer: เปิดปั๊ม (ครั้งแรกของวัน)");

                    // Publish MQTT: Timer เริ่มทำงานแล้ววันนี้
                    if (!last_published_executed_status) {
                        client.publish("ptk/esp8266/timer/executed-today", "YES", true);
                        last_published_executed_status = true;
                    }
                } else {
                    Serial.printf("❌ Timer: ไม่เปิดปั๊ม - SeaDown:%d(%s) ParkUp:%d(%s)\n",
                                  sensor->getSeaDownStatus(),
                                  sensor->getSeaDownStatus() == 1 ? "มีน้ำ" : "หมด",
                                  sensor->getParkUpStatus(),
                                  sensor->getParkUpStatus() == 1 ? "ว่าง" : "เต็ม");
                }
            }
            // ทำงานไปแล้ว และยังไม่ถูกหยุดชั่วคราว → ทำงานต่อเนื่อง
            else if (flag_timer_executed_today && !flag_keep_timer_pump_working) {
                if (shouldPumpRun) {
                    timerActive = true;  // ✅ ทำงานต่อเนื่อง
                    // ไม่ต้อง Serial.println เพราะจะ spam เยอะ
                } else {
                    // น้ำเต็มสวน หรือน้ำหมดคลอง → หยุดชั่วคราว
                    Serial.println("Timer: หยุดปั๊ม (น้ำเต็มหรือน้ำคลองหมด)");
                    flag_keep_timer_pump_working = true;  // หยุดไปจนกว่าจะหมดช่วงเวลา
                    timerActive = false;
                }
            }
        }
        // เลยช่วงเวลาแล้ว → reset flag สำหรับรอบถัดไป
        else if (currentTimeMinutes >= stopTimeMinutes) {
            if (flag_keep_timer_pump_working) {
                Serial.println("Timer: หมดช่วงเวลา → reset flag");
            }
            flag_keep_timer_pump_working = false;
            timerActive = false;
        }
    } else {
        // วันนี้ไม่ได้เปิด timer
        static unsigned long lastDayDisabledLog = 0;
        if (millis() - lastDayDisabledLog > 10000) {  // Log ทุก 10 วินาที
            Serial.printf("⚠️ Timer: วันนี้ไม่เปิด (Day:%d=%s)\n", currentDay, dayTH[currentDay]);
            lastDayDisabledLog = millis();
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
