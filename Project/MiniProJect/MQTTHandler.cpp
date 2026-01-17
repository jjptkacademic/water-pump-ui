#include "MQTTHandler.h"
#include "config.h"

MQTTHandler::MQTTHandler(PubSubClient* mqttClient, PumpController* pump, TimeManager* timer, WaterSensor* sensor) {
    client = mqttClient;
    pumpCtrl = pump;
    timeMgr = timer;
    waterSensor = sensor;
    flag_set_automatic_Check = false;
}

void MQTTHandler::init() {
    client->setServer(mqtt_server, mqtt_port);
}

void MQTTHandler::handleCallback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print(" : ");
    Serial.print(message);
    Serial.println("] ");
    
    String topicStr = String(topic);
    
    // Button control
    if (topicStr == "ptk/esp8266/btn") {
        if (message == "Btn_ON") {
            pumpCtrl->setButtonStatus(true);
            Serial.println("btn on");
            if (client->connected())
                client->publish("ptk/esp8266/deug", "LED is ON");
        } else if (message == "Btn_OFF") {
            pumpCtrl->setButtonStatus(false);
            Serial.println("btn off");
            if (client->connected())
                client->publish("ptk/esp8266/deug", "LED is OFF");
        }
    }
    // Debug mode
    else if (topicStr == "ptk/esp8266/set-debug") {
        if (message == "D_ON") {
            timeMgr->setDebugMode(true);
            Serial.println("D_ON");
            if (client->connected())
                client->publish("ptk/esp8266/deug", "Debug is ON");
        } else if (message == "D_OFF") {
            timeMgr->setDebugMode(false);
            Serial.println("D_OFF");
            if (client->connected())
                client->publish("ptk/esp8266/deug", "Debug is OFF");
        }
    }
    // Auto mode
    else if (topicStr == "ptk/esp8266/set-auto") {
        if (message == "Auto_ON") {
            flag_set_automatic_Check = true;
            Serial.println("Auto_ON");
            if (client->connected())
                client->publish("ptk/esp8266/deug", "Auto is ON");
        } else if (message == "Auto_OFF") {
            flag_set_automatic_Check = false;
            Serial.println("Auto_OFF");
            if (client->connected())
                client->publish("ptk/esp8266/deug", "Auto is OFF");
        }
    }
    // Timer mode enable/disable
    else if (topicStr == "ptk/esp8266/set-timer") {
        if (message == "Timer_ON") {
            timeMgr->setTimerMode(true);
            Serial.println("Timer Mode: ON");
            if (client->connected())
                client->publish("ptk/esp8266/deug", "Timer is ON");
        } else if (message == "Timer_OFF") {
            timeMgr->setTimerMode(false);
            Serial.println("Timer Mode: OFF");
            if (client->connected())
                client->publish("ptk/esp8266/deug", "Timer is OFF");
        }
    }
    // Timer start/stop
    else if (topicStr == "ptk/esp8266/timerstart") {
        int hour = message.substring(0, 2).toInt();
        int minute = message.substring(3, 5).toInt();
        timeMgr->setStartTime(hour * 60 + minute);
        Serial.printf("timestart %s\n", message.c_str());
    } else if (topicStr == "ptk/esp8266/timerstop") {
        int hour = message.substring(0, 2).toInt();
        int minute = message.substring(3, 5).toInt();
        timeMgr->setStopTime(hour * 60 + minute);
        Serial.printf("timestop %s\n", message.c_str());
    }
    // Days selection
    else if (topicStr == "ptk/esp8266/timer/mon") {
        timeMgr->setDayOn(0, message == "Mon_ON");
        Serial.printf("%s\n", message.c_str());
    } else if (topicStr == "ptk/esp8266/timer/tues") {
        timeMgr->setDayOn(1, message == "Tues_ON");
        Serial.printf("%s\n", message.c_str());
    } else if (topicStr == "ptk/esp8266/timer/wed") {
        timeMgr->setDayOn(2, message == "Wed_ON");
        Serial.printf("%s\n", message.c_str());
    } else if (topicStr == "ptk/esp8266/timer/thurs") {
        timeMgr->setDayOn(3, message == "Thurs_ON");
        Serial.printf("%s\n", message.c_str());
    } else if (topicStr == "ptk/esp8266/timer/fri") {
        timeMgr->setDayOn(4, message == "Fri_ON");
        Serial.printf("%s\n", message.c_str());
    } else if (topicStr == "ptk/esp8266/timer/sat") {
        timeMgr->setDayOn(5, message == "Sat_ON");
        Serial.printf("%s\n", message.c_str());
    } else if (topicStr == "ptk/esp8266/timer/sun") {
        timeMgr->setDayOn(6, message == "Sun_ON");
        Serial.printf("%s\n", message.c_str());
    }
    // ==================== ULTRASONIC THRESHOLD CONTROL ====================
    else if (topicStr == "ptk/esp8266/ultrasonic/threshold/park-start") {
        float threshold = message.toFloat();
        if (threshold > 0 && threshold < 100) {
            waterSensor->updateParkStartThreshold(threshold);
            Serial.printf("Park start threshold: %.1f%%\n", threshold);
            if (client->connected())
                client->publish("ptk/esp8266/debug", "Park start threshold updated", false);
        }
    }
    else if (topicStr == "ptk/esp8266/ultrasonic/threshold/park-stop") {
        float threshold = message.toFloat();
        if (threshold > 0 && threshold <= 100) {
            waterSensor->updateParkStopThreshold(threshold);
            Serial.printf("Park stop threshold: %.1f%%\n", threshold);
            if (client->connected())
                client->publish("ptk/esp8266/debug", "Park stop threshold updated", false);
        }
    }
    else if (topicStr == "ptk/esp8266/ultrasonic/threshold/pub-min") {
        float threshold = message.toFloat();
        if (threshold >= 0 && threshold < 100) {
            waterSensor->updatePubMinThreshold(threshold);
            Serial.printf("Pub min threshold: %.1f%%\n", threshold);
            if (client->connected())
                client->publish("ptk/esp8266/debug", "Pub min threshold updated", false);
        }
    }
    // ==================== TIMER CONTROL ====================
    else if (topicStr == "ptk/esp8266/timer/reset-flag") {
        if (message == "RESET") {
            timeMgr->resetTimerExecutionFlag();
            Serial.println("Timer execution flag reset - สามารถทำงานซ้ำได้");
            if (client->connected())
                client->publish("ptk/esp8266/debug", "Timer flag reset", false);
        }
    }
}

void MQTTHandler::reconnect() {
    while (!client->connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        
        if (client->connect(clientId.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("connected");
            
            // Subscribe to topics
            client->subscribe("ptk/esp8266/timerstart");
            client->subscribe("ptk/esp8266/timerstop");
            client->subscribe("ptk/esp8266/timer/mon");
            client->subscribe("ptk/esp8266/timer/tues");
            client->subscribe("ptk/esp8266/timer/wed");
            client->subscribe("ptk/esp8266/timer/thurs");
            client->subscribe("ptk/esp8266/timer/fri");
            client->subscribe("ptk/esp8266/timer/sat");
            client->subscribe("ptk/esp8266/timer/sun");
            client->subscribe("ptk/esp8266/set-debug");
            client->subscribe("ptk/esp8266/set-auto");
            client->subscribe("ptk/esp8266/set-timer");  // Timer mode enable/disable
            client->subscribe("ptk/esp8266/water-level-park");
            client->subscribe("ptk/esp8266/water-level-pub");
            client->subscribe("ptk/esp8266/btn");

            // Ultrasonic threshold control topics
            client->subscribe("ptk/esp8266/ultrasonic/threshold/park-start");
            client->subscribe("ptk/esp8266/ultrasonic/threshold/park-stop");
            client->subscribe("ptk/esp8266/ultrasonic/threshold/pub-min");

            // Timer control topics
            client->subscribe("ptk/esp8266/timer/reset-flag");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client->state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void MQTTHandler::loop() {
    if (!client->connected()) {
        reconnect();
    }
    client->loop();
}
