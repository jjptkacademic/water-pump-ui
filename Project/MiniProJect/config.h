#ifndef CONFIG_H
#define CONFIG_H

// ==================== PIN DEFINITIONS ====================
// Controls
#define Wather_Pump   D7   // GPIO13 - Water Pump (HIGH=ON, LOW=OFF)

// LED Indicators
#define LED_Status    D0   // GPIO16 - Single Status LED

// Digital Water Sensors (3 sensors)
#define SensorinSeaUp     D3   // GPIO0 - Sea/Pub tank upper sensor (1=Empty, 0=Water)
#define SensorinSeaDown   D4   // GPIO2 - Sea/Pub tank lower sensor (0=Empty, 1=Water)
#define SensorinParkUp    D8   // GPIO15 - Park tank upper sensor (1=Empty, 0=Water)
// SensorinParkDown removed - replaced by ultrasonic

// ==================== ULTRASONIC SENSORS ====================
// Park Tank (วัดน้ำสวน 0-100%)
#define ULTRASONIC_PARK_TRIG  D1   // GPIO5 - Trigger pin
#define ULTRASONIC_PARK_ECHO  D2   // GPIO4 - Echo pin

// Pub/Sea Tank (วัดน้ำคลอง 0-100%)
#define ULTRASONIC_PUB_TRIG   D5   // GPIO14 - Trigger pin
#define ULTRASONIC_PUB_ECHO   D6   // GPIO12 - Echo pin

// ==================== ULTRASONIC CALIBRATION ====================
// Park Tank calibration values (measure these with CalibrateUltrasonic.ino)
#define PARK_DISTANCE_EMPTY   28.92  // cm - distance when tank is empty
#define PARK_DISTANCE_FULL    14.2   // cm - distance when tank is full

// Pub/Sea Tank calibration values
#define PUB_DISTANCE_EMPTY    42.0  // cm - distance when tank is empty
#define PUB_DISTANCE_FULL     23.0   // cm - distance when tank is full

// ==================== ULTRASONIC THRESHOLDS ====================
// Auto mode will start pump when Park < START and Pub > MIN
// Auto mode will stop pump when Park >= STOP or Pub < MIN
#define PARK_START_THRESHOLD  30.0   // % - start pump when park water < 30%
#define PARK_STOP_THRESHOLD   90.0   // % - stop pump when park water >= 90%
#define PUB_MIN_THRESHOLD     50.0   // % - minimum water level in pub tank

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
