// Test Command Relay with Float Sensor
// D8 = SensorinParkUp (Park tank upper sensor)
// D7 = Relay (Active LOW - LOW=ON, HIGH=OFF)

const int SensorinParkUp = D8;    // Park tank upper sensor
const int RelayPin = D7;          // Relay control pin (Active LOW)

void setup() {
  Serial.begin(115200);

  // Set pin modes
  pinMode(SensorinParkUp, INPUT);
  pinMode(RelayPin, OUTPUT);

  // Turn OFF relay initially (Active LOW -> HIGH = OFF)
  digitalWrite(RelayPin, LOW); // HIGH = ON  , Default = LOW = OFF

  delay(1000);
  Serial.println("\n\n=================================");
  Serial.println("Test Command Relay with Float Sensor");
  Serial.println("=================================");
  Serial.println("Logic:");
  Serial.println("- Sensor = 0 (has water) -> Relay ON");
  Serial.println("- Sensor = 1 (no water)  -> Relay OFF");
  Serial.println("=================================\n");
}

void loop() {
  // Read sensor value
  int sensorValue = digitalRead(SensorinParkUp);

  // Control relay based on sensor
  if (sensorValue == 0) {
    // Has water -> Turn ON relay (Active LOW)
    digitalWrite(RelayPin, LOW);
  } else {
    // No water -> Turn OFF relay (Active LOW)
    digitalWrite(RelayPin, HIGH);  // HIGH = ON
  }

  // Display status
  Serial.println("+------------------+-------+-------------+");
  Serial.println("| Component        | Value | Status      |");
  Serial.println("+------------------+-------+-------------+");

  Serial.print("| D8: Sensor       |   ");
  Serial.print(sensorValue);
  Serial.print("   | ");
  if (sensorValue == 0) {
    Serial.println("Has water   |");
  } else {
    Serial.println("No water    |");
  }

  Serial.print("| D7: Relay        |   ");
  Serial.print(digitalRead(RelayPin));
  Serial.print("   | ");
  if (digitalRead(RelayPin) == LOW) {
    Serial.println("ON (Active) |");
  } else {
    Serial.println("OFF         |");
  }

  Serial.println("+------------------+-------+-------------+");

  Serial.print("\nSummary: Sensor=");
  Serial.print(sensorValue == 0 ? "Water" : "Empty");
  Serial.print(" | Relay=");
  Serial.println(digitalRead(RelayPin) == LOW ? "ON" : "OFF");

  Serial.println("\n");

  delay(1000);  // Update every 1 second
}
