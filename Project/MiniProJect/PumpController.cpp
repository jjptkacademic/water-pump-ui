#include "PumpController.h"
#include "config.h"
#include <PubSubClient.h>

extern PubSubClient client;

PumpController::PumpController(WaterSensor* waterSensor) {
    sensor = waterSensor;
    pump_working = false;
    flag_autopump_on = false;
    flag_send_pub_to_led_status = false;
    Button_Status = false;
    flag_trigBtn_start = false;
    flag_timer_pump = false;
}

void PumpController::init() {
    pinMode(Wather_Pump, OUTPUT);
    pinMode(ButtonOn, INPUT);
    pinMode(ButtonOff, INPUT);
    digitalWrite(Wather_Pump, LOW);
}

void PumpController::openPump() {
    if (!pump_working && client.connected()) {
        digitalWrite(Wather_Pump, HIGH);
        client.publish("ptk/esp8266/status", "Led_ON", true);
        client.publish("ptk/esp8266/btn", "Btn_ON", true);
        Serial.println("ปั๊มทำงาน");
    }
    flag_send_pub_to_led_status = false;
    pump_working = true;
}

void PumpController::offPump() {
    digitalWrite(Wather_Pump, LOW);
    pump_working = false;
    if (!flag_send_pub_to_led_status && client.connected()) {
        client.publish("ptk/esp8266/status", "Led_OFF", true);
        client.publish("ptk/esp8266/btn", "Btn_OFF", true);
        flag_send_pub_to_led_status = true;
        Serial.println("ปั๊มไม่ทำงาน");
    }
}

bool PumpController::checkPumpWorking() {
    // ถ้าเซนเซอร์บน ในสวนน้ำเต็ม หรือ น้ำในคลองหมด
    if (sensor->shouldStopPump()) {
        flag_autopump_on = false;
        Serial.println("น้ำหมดละจ้า");
        return false;
    }
    // ถ้าถึงเวลาในการปั๊มน้ำ
    else if (flag_timer_pump) {
        Serial.println("ทำงานตามเวลาอยู่ครับพี่");
        return true;
    }
    // ถ้าปุ่มถูกกด
    else if (flag_trigBtn_start) {
        Serial.println("ทำงานตามที่กดปุ่มมาอยู่ครับนาย");
        return true;
    }
    // ถ้าเซนเซอร์ตัวล่างในสวน ไม่มีน้ำ และมีน้ำในแหล่งน้ำคลอง
    else if (sensor->getParkDownStatus() == 1 && sensor->getParkUpStatus() == 0 && sensor->isWaterFullInPub()) {
        Serial.println("ทำงานอัตโนมัติตามเงื่อนไขอยู่เลย");
        flag_autopump_on = true;
        return true;
    }
    // ถ้ายังมี flag true ให้ปั๊มทำงานต่อไป
    else if (flag_autopump_on) {
        Serial.println("flag ทำงานอัตโนมัติ");
        return true;
    }
    
    return false;
}

void PumpController::checkButtonPump() {
    if (!client.connected()) return;
    
    // เช็คว่ามีการกดปุ่ม และ ยังมีน้ำในคลอง และ น้ำยังไม่เต็มสวน
    if (Button_Status && !sensor->shouldStopPump()) {
        Serial.println("Return true Her");
        flag_trigBtn_start = true;
    }
    // ถ้าปุ่มเปิดแล้ว แต่น้ำเต็มสวน หรือน้ำหมดคลอง
    else if (flag_trigBtn_start && sensor->shouldStopPump()) {
        Serial.println("Return false Her");
        flag_trigBtn_start = false;
    }
    // ถ้าไม่มีการกดปุ่ม
    else if (!Button_Status) {
        flag_trigBtn_start = false;
    }
    // ถ้ากดปุ่มแล้วถูกหยุด จะ set ปุ่มให้เด้งกลับไปปิด
    else if (Button_Status && !flag_trigBtn_start) {
        Button_Status = false;
        Serial.println("btn off เพราะไม่มีน้ำ");
        if (client.connected()) {
            client.publish("ptk/esp8266/deug", "LED is OFF");
            client.publish("ptk/esp8266/btn", "Btn_OFF", true);
        }
    }
}

void PumpController::checkAutoPump(bool autoEnabled) {
    if (!client.connected()) return;
    
    if (autoEnabled) {
        if (checkPumpWorking()) {
            openPump();
        } else {
            offPump();
        }
    } else {
        if (!flag_send_pub_to_led_status) {
            client.publish("ptk/esp8266/status", "Led_OFF", true);
            client.publish("ptk/esp8266/btn", "Btn_OFF", true);
            flag_send_pub_to_led_status = true;
            Serial.println("ปั๊มไม่ทำงาน");
            pump_working = false;
        }
    }
}
