#include "config.h"

// WiFi Configuration
const char* ssid = "Nebula";
const char* password = "teacherRoom";

// MQTT Configuration
const char* mqtt_server = "6555a54274d6454ab49e8eb9b94c325c.s1.eu.hivemq.cloud";
const char* mqtt_username = "esp8266ptk";
const char* mqtt_password = "Aa12341234";
const int mqtt_port = 8883;

// NTP Configuration
const long utcOffsetInSeconds = 25200;  // UTC+7 (Thailand)
const unsigned long NTP_INTERVAL = 3600000;  // 1 hour
