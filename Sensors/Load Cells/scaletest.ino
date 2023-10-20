#include "HX711.h"

const int LOADCELL_DOUT_PIN = 22;
const int LOADCELL_SCK_PIN = 23;

const int LOADCELL_DOUT_PIN2 = 37;
const int LOADCELL_SCK_PIN2 = 38;

HX711 scale, scale2;

unsigned long loopStartTime; // Variable to store the start time of the loop

void setup() {
  Serial.begin(500000);

  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale2.begin(LOADCELL_DOUT_PIN2, LOADCELL_SCK_PIN2);

  Serial.println("Taring the scale");
  scale.set_scale(2280.f);  // Set the scale factor for conversion to kilograms
  scale.tare();             // Reset the scale to zero

  scale2.set_scale(2280.f); // Set the scale factor for conversion to kilograms
  scale2.tare();            // Reset the scale to zero

  Serial.println("After taring the scale:");
}

void loop() {
  loopStartTime = micros(); // Record the start time of the loop

  // Reading the weight from both load cells
  float weight1 = scale.get_units(1);  // Get the weight in kilograms
  float weight2 = scale2.get_units(1); // Get the weight in kilograms

  // Print the readings
  Serial.print("Reading 1: ");
  Serial.print(weight1, 1);
  Serial.println(" kg");

  Serial.print("Reading 2: ");
  Serial.print(weight2, 1);
  Serial.println(" kg");

  // Calculate and print the loop duration
  unsigned long loopEndTime = micros(); // Record the end time of the loop
  unsigned long loopDuration = loopEndTime - loopStartTime;

  Serial.print("Loop duration: ");
  Serial.print(loopDuration);
  Serial.println(" us");

  // Delay to achieve desired loop time
  if (loopDuration < 10000) {
    delayMicroseconds(10000 - loopDuration);
  }
}
