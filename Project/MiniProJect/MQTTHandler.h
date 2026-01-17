#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include "PumpController.h"
#include "TimeManager.h"
#include "WaterSensor.h"

class MQTTHandler {
private:
    PubSubClient* client;
    PumpController* pumpCtrl;
    TimeManager* timeMgr;
    WaterSensor* waterSensor;
    bool flag_set_automatic_Check;

public:
    MQTTHandler(PubSubClient* mqttClient, PumpController* pump, TimeManager* timer, WaterSensor* sensor);
    void init();
    void handleCallback(char* topic, byte* payload, unsigned int length);
    void reconnect();
    void loop();

    // Auto mode
    void setAutoMode(bool enabled) { flag_set_automatic_Check = enabled; }
    bool getAutoMode() { return flag_set_automatic_Check; }
};

#endif
