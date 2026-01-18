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

    // LED indicators
    pinMode(LED_Status, OUTPUT);

    digitalWrite(Wather_Pump, LOW); // Default OFF
    digitalWrite(LED_Status, LOW);  // LED off
}

void PumpController::openPump() {
    if (!pump_working && client.connected()) {
        digitalWrite(Wather_Pump, HIGH); // HIGH = ON
        client.publish("ptk/esp8266/status", "Led_ON", true);
        client.publish("ptk/esp8266/btn", "Btn_ON", true);
        Serial.println("ปั๊มทำงาน");
        
        // Turn on Status LED
        digitalWrite(LED_Status, HIGH);
    }
    flag_send_pub_to_led_status = false;
    pump_working = true;
}

void PumpController::offPump() {
    digitalWrite(Wather_Pump, LOW); // LOW = OFF
    pump_working = false;
    
    // Turn off Status LED
    digitalWrite(LED_Status, LOW);
    
    if (!flag_send_pub_to_led_status && client.connected()) {
        client.publish("ptk/esp8266/status", "Led_OFF", true);
        client.publish("ptk/esp8266/btn", "Btn_OFF", true);
        flag_send_pub_to_led_status = true;
        Serial.println("ปั๊มไม่ทำงาน");
    }
}

bool PumpController::checkPumpWorking() {
    // Priority 1: Safety check (digital + ultrasonic)
    if (sensor->shouldStopPump()) {
        flag_autopump_on = false;
        Serial.println("หยุดปั๊ม: น้ำเต็มสวนหรือน้ำหมดคลอง");
        return false;
    }

    // Priority 2: Timer mode
    if (flag_timer_pump) {
        Serial.println("ทำงานตามเวลาอยู่ครับพี่");
        return true;
    }

    // Priority 3: Button mode
    if (flag_trigBtn_start) {
        Serial.println("ทำงานตามที่กดปุ่มมาอยู่ครับนาย");
        return true;
    }

    // Priority 4: Auto mode - Try ultrasonic first
    if (sensor->shouldStartPumpUltrasonic()) {
        Serial.println("เปิดปั๊มอัตโนมัติ: Ultrasonic (น้ำสวนต่ำ)");
        flag_autopump_on = true;
        return true;
    }

    // Priority 5: Continue pumping if auto flag is set
    if (flag_autopump_on) {
        Serial.println("ปั๊มทำงานต่อเนื่อง (auto mode)");
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
            // Ensure LED is off
            digitalWrite(LED_Status, LOW);
        }
    }
}
