// Float Sensor Test - 3 Sensors
// D1 = SensorinSeaUp (Sea tank upper sensor)
// D2 = SensorinSeaDown (Sea tank lower sensor)
// D3 = SensorinParkUp (Park tank upper sensor)

const int SensorinSeaUp = D3;     // Sea tank upper sensor
const int SensorinSeaDown = D4;   // Sea tank lower sensor
const int SensorinParkUp = D8;    // Park tank upper sensor

void setup() {
  Serial.begin(115200);

  // Set pins as INPUT_PULLUP (using internal pull-up resistor)
  pinMode(SensorinSeaUp, INPUT);
  pinMode(SensorinSeaDown, INPUT);
  pinMode(SensorinParkUp, INPUT);

  delay(1000);
  Serial.println("\n\n=================================");
  Serial.println("Float Sensor Test - 3 Sensors");
  Serial.println("=================================");
  Serial.println("Note:");
  Serial.println("- D1, D3: LOW=Water, HIGH=No water");
  Serial.println("- D2: HIGH=Water, LOW=No water (inverted)");
  Serial.println("=================================\n");
}

void loop() {
  // Read values from all 3 sensors
  int seaUp = digitalRead(SensorinSeaUp);
  int seaDown = digitalRead(SensorinSeaDown);
  int parkUp = digitalRead(SensorinParkUp);

  // Display as table
  Serial.println("+---------------------+-------+--------------+");
  Serial.println("| Sensor              | Value | Status       |");
  Serial.println("+---------------------+-------+--------------+");

  // D1: SensorinSeaUp
  Serial.print("| D1: SeaUp           |   ");
  Serial.print(seaUp);
  Serial.print("   | ");
  if (seaUp == LOW) {
    Serial.println("Has water    |");
  } else {
    Serial.println("No water     |");
  }

  // D2: SensorinSeaDown (inverted logic - NC or reversed wiring)
  Serial.print("| D2: SeaDown         |   ");
  Serial.print(seaDown);
  Serial.print("   | ");
  if (seaDown == HIGH) {
    Serial.println("Has water    |");
  } else {
    Serial.println("No water     |");
  }

  // D3: SensorinParkUp
  Serial.print("| D3: ParkUp          |   ");
  Serial.print(parkUp);
  Serial.print("   | ");
  if (parkUp == LOW) {
    Serial.println("Has water    |");
  } else {
    Serial.println("No water     |");
  }

  Serial.println("+---------------------+-------+--------------+");

  // Summary status
  Serial.print("\nSummary: ");
  Serial.print("SeaUp=");
  Serial.print(seaUp == LOW ? "Water" : "Empty");
  Serial.print(" | SeaDown=");
  Serial.print(seaDown == HIGH ? "Water" : "Empty");  // Inverted for D2
  Serial.print(" | ParkUp=");
  Serial.println(parkUp == LOW ? "Water" : "Empty");

  Serial.println("\n");

  delay(1000);  // Update every 1 second
}
