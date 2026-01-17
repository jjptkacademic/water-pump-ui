// Include libraries
#include <TaskScheduler.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Include custom headers
#include "config.h"
#include "UltrasonicSensor.h"
#include "WaterSensor.h"
#include "PumpController.h"
#include "TimeManager.h"
#include "MQTTHandler.h"

// Global objects
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Ultrasonic sensors
UltrasonicSensor ultrasonicPark(ULTRASONIC_PARK_TRIG, ULTRASONIC_PARK_ECHO);
UltrasonicSensor ultrasonicPub(ULTRASONIC_PUB_TRIG, ULTRASONIC_PUB_ECHO);

// Create objects
WaterSensor waterSensor;
PumpController pumpController(&waterSensor);
TimeManager timeManager(&timeClient, &waterSensor);
MQTTHandler mqttHandler(&client, &pumpController, &timeManager, &waterSensor);

// Scheduler
Scheduler runner;

// WiFi setup
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// MQTT Callback wrapper
void callback(char* topic, byte* payload, unsigned int length) {
  mqttHandler.handleCallback(topic, payload, length);
}

// Task callbacks
void SensorRead() {
  waterSensor.read();              // Read digital sensors
  waterSensor.readUltrasonic();    // Read ultrasonic sensors
  waterSensor.publishLevels(timeManager.getDebugMode());
}

void Check_Auto_Pump() {
  pumpController.checkAutoPump(mqttHandler.getAutoMode());
}

void Check_Btn_Pump() {
  pumpController.checkButtonPump();
}

void Check_Timer_Pump() {
  bool timerActive = timeManager.checkTimerPump();
  pumpController.setTimerFlag(timerActive);
}

// สร้าง Tasks (timer , infinity , function)
Task sensorReadTask(SENSOR_READ_INTERVAL, TASK_FOREVER, &SensorRead);
Task checkPumpTask(PUMP_CHECK_INTERVAL, TASK_FOREVER, &Check_Auto_Pump);
Task btnTask(BTN_CHECK_INTERVAL, TASK_FOREVER, &Check_Btn_Pump);
Task timerTask(TIMER_CHECK_INTERVAL, TASK_FOREVER, &Check_Timer_Pump);

void setup() { // all setup
  Serial.begin(115200);

  // Initialize components
  waterSensor.init();
  pumpController.init();

  // ==================== INITIALIZE ULTRASONIC SENSORS ====================
  Serial.println("Initializing ultrasonic sensors...");

  // Initialize ultrasonic hardware
  ultrasonicPark.init();
  ultrasonicPub.init();

  // Set calibration values from config.h
  ultrasonicPark.setCalibration(PARK_DISTANCE_EMPTY, PARK_DISTANCE_FULL);
  ultrasonicPub.setCalibration(PUB_DISTANCE_EMPTY, PUB_DISTANCE_FULL);

  // Link ultrasonic sensors to water sensor
  waterSensor.initUltrasonic(&ultrasonicPark, &ultrasonicPub);

  // Set thresholds from config.h
  waterSensor.setThresholds(PARK_START_THRESHOLD, PARK_STOP_THRESHOLD, PUB_MIN_THRESHOLD);

  Serial.println("Ultrasonic sensors initialized");
  Serial.printf("Park: Empty=%.1fcm, Full=%.1fcm\n", PARK_DISTANCE_EMPTY, PARK_DISTANCE_FULL);
  Serial.printf("Pub: Empty=%.1fcm, Full=%.1fcm\n", PUB_DISTANCE_EMPTY, PUB_DISTANCE_FULL);
  Serial.printf("Thresholds: Start=%.1f%%, Stop=%.1f%%, PubMin=%.1f%%\n",
                PARK_START_THRESHOLD, PARK_STOP_THRESHOLD, PUB_MIN_THRESHOLD);

  // Setup WiFi
  setup_wifi();
  
  // Initialize time manager
  timeManager.init();
  
  // Setup MQTT
  espClient.setInsecure();
  mqttHandler.init();
  client.setCallback(callback);
  
  // เพิ่ม tasks เข้าไปใน scheduler
  runner.addTask(checkPumpTask);
  runner.addTask(btnTask);
  runner.addTask(sensorReadTask);
  runner.addTask(timerTask);
  
  // เริ่มต้นการทำงานของ tasks
  checkPumpTask.enable();
  btnTask.enable();
  sensorReadTask.enable();
  timerTask.enable();
  
  Serial.println("ระบบเริ่มต้นทำงานแล้ว");
}

void loop() {
  mqttHandler.loop();
  runner.execute();
}
