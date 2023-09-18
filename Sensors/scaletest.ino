  
/**
 *
 * HX711 library for Arduino - example file
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/
#include "HX711.h"


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 4;

const int LOADCELL_DOUT_PIN2 = 28;
const int LOADCELL_SCK_PIN2 = 29;

const int LOADCELL_DOUT_PIN3 = 22;
const int LOADCELL_SCK_PIN3 = 23;

const int LOADCELL_DOUT_PIN4 = 37;
const int LOADCELL_SCK_PIN4 = 38;

HX711 scale, scale2, scale3, scale4;

void setup() {
  Serial.begin(500000);
  Serial.println("HX711 Demo");

  Serial.println("Initializing the scale");

  // Initialize library with data output pin, clock input pin and gain factor.
  // Channel selection is made by passing the appropriate gain:
  // - With a gain factor of 64 or 128, channel A is selected
  // - With a gain factor of 32, channel B is selected
  // By omitting the gain factor parameter, the library
  // default "128" (Channel A) is used here.
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale2.begin(LOADCELL_DOUT_PIN2, LOADCELL_SCK_PIN2);
  scale3.begin(LOADCELL_DOUT_PIN3, LOADCELL_SCK_PIN3);
  scale4.begin(LOADCELL_DOUT_PIN4, LOADCELL_SCK_PIN4);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.print(scale.read());      // print a raw reading from the ADC
  Serial.println(scale2.read());      // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.print(scale.read_average(20));   // print the average of 20 readings from the ADC
  Serial.println(scale2.read_average(20));   // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.print(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)
  Serial.println(scale2.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.print(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
            // by the SCALE parameter (not set yet)
  Serial.print(scale2.get_units(5), 1);

  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0
  scale2.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale2.tare();    
  scale3.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale3.tare();    
  scale4.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale4.tare();    

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.print(scale.read());                 // print a raw reading from the ADC
  Serial.println(scale2.read()); 

  Serial.print("read average: \t\t");
  Serial.print(scale.read_average(20));       // print the average of 20 readings from the ADC
  Serial.println(scale2.read_average(20));
  
  Serial.print("get value: \t\t");
  Serial.print(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()
  Serial.println(scale2.get_value(5));
  
  Serial.print("get units: \t\t");
  Serial.print(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
            // by the SCALE parameter set with set_scale
  Serial.println(scale2.get_units(5), 1);

  Serial.println("Readings:");
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print(scale2.get_units(), 1);
  //delay(5000);
  scale.set_scale();
  scale.tare();
  scale2.set_scale();
  scale2.tare();
  scale3.set_scale();
  scale3.tare();
  scale4.set_scale();
  scale4.tare();
  float diviation = (scale.get_units(), 10);
  Serial.println(diviation);
  diviation = diviation/.25;
  scale.set_scale(diviation);
  float diviation2 = (scale2.get_units(), 10);
  Serial.println(diviation2);
  diviation2 = diviation2/.25;
  scale2.set_scale(diviation2);
  float diviation3 = (scale3.get_units(), 10);
  Serial.println(diviation3);
  diviation3 = diviation3/.25;
  scale3.set_scale(diviation3);
  float diviation4 = (scale4.get_units(), 10);
  Serial.println(diviation4);
  diviation4 = diviation4/.25;
  scale4.set_scale(diviation4);
}

void loop() {
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print(scale3.get_units(), 1);
  Serial.print(scale4.get_units(), 1);
  Serial.println(scale2.get_units(), 1);
  
//  set_scale();
//  tare();
//  Serial.println(scale.get_units(), 10);
//  delay(10000);


  
//  Serial.print("\t| average:\t");
//  Serial.println(scale.get_units(10), 1);

}
