#ifndef CONFIG_H
#define CONFIG_H

// ==================== PIN DEFINITIONS ====================
// Controls
#define Wather_Pump   D7   // GPIO13 - Water Pump

// LED Indicators
#define LED_Green     A0   // ADC - Green LED (moved from D5)
#define LED_Red       D6   // GPIO12 - Red LED

// Digital Water Sensors (3 sensors)
#define SensorinParkUp    D3   // GPIO0 - Park tank upper sensor
#define SensorinSeaDown   D2   // GPIO4 - Sea/Pub tank lower sensor
#define SensorinSeaUp     D1   // GPIO5 - Sea/Pub tank upper sensor
// SensorinParkDown removed - replaced by ultrasonic

// ==================== ULTRASONIC SENSORS ====================
// Park Tank (วัดน้ำสวน 0-100%)
#define ULTRASONIC_PARK_TRIG  D0   // GPIO16 - Trigger pin
#define ULTRASONIC_PARK_ECHO  D5   // GPIO14 - Echo pin

// Pub/Sea Tank (วัดน้ำคลอง 0-100%)
#define ULTRASONIC_PUB_TRIG   D8   // GPIO15 - Trigger pin
#define ULTRASONIC_PUB_ECHO   D4   // GPIO2 - Echo pin

// ==================== ULTRASONIC CALIBRATION ====================
// Park Tank calibration values (measure these with CalibrateUltrasonic.ino)
#define PARK_DISTANCE_EMPTY   150.0  // cm - distance when tank is empty
#define PARK_DISTANCE_FULL    20.0   // cm - distance when tank is full

// Pub/Sea Tank calibration values
#define PUB_DISTANCE_EMPTY    200.0  // cm - distance when tank is empty
#define PUB_DISTANCE_FULL     30.0   // cm - distance when tank is full

// ==================== ULTRASONIC THRESHOLDS ====================
// Auto mode will start pump when Park < START and Pub > MIN
// Auto mode will stop pump when Park >= STOP or Pub < MIN
#define PARK_START_THRESHOLD  30.0   // % - start pump when park water < 30%
#define PARK_STOP_THRESHOLD   90.0   // % - stop pump when park water >= 90%
#define PUB_MIN_THRESHOLD     20.0   // % - minimum water level in pub tank

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
