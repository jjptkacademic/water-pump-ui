// Include libraries
#include <TaskScheduler.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Include custom headers
#include "config.h"
#include "WaterSensor.h"
#include "PumpController.h"
#include "TimeManager.h"
#include "MQTTHandler.h"

// Global objects
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Create objects
WaterSensor waterSensor;
PumpController pumpController(&waterSensor);
TimeManager timeManager(&timeClient, &waterSensor);
MQTTHandler mqttHandler(&client, &pumpController, &timeManager);

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
  waterSensor.read();
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
