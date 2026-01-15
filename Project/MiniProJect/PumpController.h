#ifndef PUMP_CONTROLLER_H
#define PUMP_CONTROLLER_H

#include <Arduino.h>
#include "WaterSensor.h"

class PumpController {
private:
    bool pump_working;
    bool flag_autopump_on;
    bool flag_send_pub_to_led_status;
    bool Button_Status;
    bool flag_trigBtn_start;
    bool flag_timer_pump;
    
    WaterSensor* sensor;

public:
    PumpController(WaterSensor* waterSensor);
    void init();
    
    // Pump control
    void openPump();
    void offPump();
    
    // Status checks
    bool isPumpWorking() { return pump_working; }
    bool checkPumpWorking();
    
    // Button control
    void setButtonStatus(bool status) { Button_Status = status; }
    bool getButtonStatus() { return Button_Status; }
    void checkButtonPump();
    
    // Auto mode
    void checkAutoPump(bool autoEnabled);
    
    // Timer mode
    void setTimerFlag(bool flag) { flag_timer_pump = flag; }
    bool getTimerFlag() { return flag_timer_pump; }
};

#endif
