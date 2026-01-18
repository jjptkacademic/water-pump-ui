// ===============================================
// HC-SR04 Ultrasonic Sensor Calibration Program
// ===============================================
// Use this to measure distance_empty and distance_full
// Then configure these values in your main program

// Pin Definitions
#define TRIG_PIN D1  // Trigger pin for HC-SR04
#define ECHO_PIN D2  // Echo pin for HC-SR04

// ===============================================
// Calibration Variables - Insert measured values here
// ===============================================
float distance_empty = 0.0 ;  // 0.0 Distance when tank is empty (cm)
float distance_full = 0.0;   //0.0  Distance when tank is full (cm)
// ===============================================

// Function to measure distance using HC-SR04
float measureDistance() {
  // Send trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo pulse
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms

  // Calculate distance (cm)
  // Speed of sound = 340 m/s or 0.034 cm/µs
  // Distance = (duration / 2) * 0.034
  float distance = duration * 0.034 / 2;

  // If no echo received (timeout)
  if (duration == 0) {
    return -1; // error
  }

  return distance;
}

// Function to calculate water level percentage
float calculateWaterLevel(float current_distance) {
  if (distance_empty == 0.0 || distance_full == 0.0) {
    return -1; // Not calibrated yet
  }

  // Calculate water level as percentage
  float water_level = ((distance_empty - current_distance) / (distance_empty - distance_full)) * 100.0;

  // Limit value to 0-100%
  if (water_level < 0) water_level = 0;
  if (water_level > 100) water_level = 100;

  return water_level;
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Setup pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("\n\n========================================");
  Serial.println("   HC-SR04 Calibration Program");
  Serial.println("========================================");
  Serial.println();
  Serial.println("Instructions:");
  Serial.println("1. Empty the tank completely, note the 'Distance' value");
  Serial.println("   -> Set this value in distance_empty variable");
  Serial.println();
  Serial.println("2. Fill the tank completely, note the 'Distance' value");
  Serial.println("   -> Set this value in distance_full variable");
  Serial.println();
  Serial.println("3. Update both values and upload again");
  Serial.println("   You will see 'Water Level' displayed in %");
  Serial.println("========================================");
  Serial.println();

  if (distance_empty != 0.0 && distance_full != 0.0) {
    Serial.println("Status: CALIBRATED");
    Serial.printf("  - Empty distance: %.2f cm\n", distance_empty);
    Serial.printf("  - Full distance:  %.2f cm\n", distance_full);
    Serial.printf("  - Tank height:    %.2f cm\n", distance_empty - distance_full);
    Serial.println();
  } else {
    Serial.println("Status: NOT CALIBRATED");
    Serial.println("  Please measure and set distance_empty and distance_full");
    Serial.println();
  }

  Serial.println("Starting measurements...\n");
  delay(1000);
}

void loop() {
  // Measure distance
  float distance = measureDistance();

  if (distance > 0) {
    Serial.print("Distance: ");
    Serial.print(distance, 2);
    Serial.print(" cm");

    // If calibrated, show water level too
    if (distance_empty != 0.0 && distance_full != 0.0) {
      float water_level = calculateWaterLevel(distance);
      Serial.print("  |  Water Level: ");
      Serial.print(water_level, 1);
      Serial.print(" %");

      // Display bar graph
      Serial.print("  [");
      int bars = (int)(water_level / 10);
      for (int i = 0; i < 10; i++) {
        if (i < bars) Serial.print("=");
        else Serial.print("-");
      }
      Serial.print("]");
    }

    Serial.println();
  } else {
    Serial.println("Error: Cannot read sensor (check wiring)");
  }

  delay(500);  // Measure every 0.5 seconds
}
