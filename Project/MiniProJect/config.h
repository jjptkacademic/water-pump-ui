#ifndef CONFIG_H
#define CONFIG_H

// Pin Definitions
#define ButtonOn D5
#define ButtonOff D6
#define SensorinParkDown D4
#define SensorinParkUp D3
#define SensorinSeaDown D2
#define SensorinSeaUp D1
#define Wather_Pump D7

// WiFi Configuration
extern const char* ssid;
extern const char* password;

// MQTT Configuration
extern const char* mqtt_server;
extern const char* mqtt_username;
extern const char* mqtt_password;
extern const int mqtt_port;

// NTP Configuration
extern const long utcOffsetInSeconds;
extern const unsigned long NTP_INTERVAL;

// Task Intervals
#define SENSOR_READ_INTERVAL 200
#define PUMP_CHECK_INTERVAL 500
#define BTN_CHECK_INTERVAL 500
#define TIMER_CHECK_INTERVAL 500

#endif
