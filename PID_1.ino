// Stepper Motor Setup
  #include "Arduino.h"
  #include "teensystep4.h"
  using namespace TS4;
  #define WINDOW_SIZE 10
 

  Stepper s1(5, 4); // indicates pin for STEP and DIR, respectively
  unsigned long motorVelocity = 200'000;
  unsigned long motorAccel = 200'000;

// Pressure Transducer Setup
  // time measurement variables
  unsigned long time_start;
  unsigned long time_stop;
  unsigned long time_pc;
  unsigned long lastUpdatePC;

  // define pins for each PT
  const int pt1_pin = 25;
  const int ptin_pin = 26;

  // PT ADC values
  const float pt1_analog_zero = 409.6/4.0; //analog reading of pressure transducer at 0psi
  const float pt1_analog_max = 3686.4/4.0; //analog reading of pressure transducer at 100psi
  const float pt1_psi_max = 3000; //max psi value of transducer being used

  const float ptin_analog_zero = 409.6/4.0; //analog reading of pressure transducer at 0psi
  const float ptin_analog_max = 3686.4/4.0; //analog reading of pressure transducer at 100psi
  const float ptin_psi_max = 3000.0; //max psi value of transducer being used

  // temp reading variables
  float pt_analog_reading1 = 0; //variable to store the value coming from the pressure transducer
  float pt_digital_reading1 = 0; //variable to store the value coming from the pressure transducer

  float pt_analog_readingin = 0; //variable to store the value coming from the pressure transducer
  float pt_digital_readingin = 0; //variable to store the value coming from the pressure transducer

  // update interval in microseconds
  unsigned long updateIntervalPC = 5000;

  // Moving Average Setup
  int INDEX = 0;
  int VALUE = 0;
  int SUM = 0;
  int READINGS[WINDOW_SIZE];
  int AVERAGED = 0;

// PID Setup
  // PID Control Variables
  float Kp = 0.5;
  float Ki = 0.0000001;
  float Kd = 2000; // change constant, could be larger
  int dt = 10000; // loop time, micros
  float loopActual = dt; // actual loop time, micros
  float target = 420; // initial target, psi
  float actual;
  float error = 0;
  float errorOld;
  float errorChange;
  float errorDerivative;
  float errorIntegral = 0;
  float actuatorSignal;
  float actuatorSaturationOpen = -18000; // saturation value dependent on open valve state, change constant
  float actuatorSaturationClosed = 0; // saturation value dependent on closed valve state
  float saturationReduction;
  float sensorDataRaw; //data from sensor
  float smoothingFactor = 0.5; //number between 0.0 and 1.0, change constant
  float sensorDataFilteredOld = 0; //
  float sensorDataFiltered; //

void setup() {
  // Pressure Transducer Setup
  Serial.begin(112500); //initializes serial communication at set baud rate bits per second, may vary based on pt

  // Stepper Motor Setup
  TS4::begin();

  s1.setMaxSpeed(200'000); // initializes maximum motor speed
  s1.setAcceleration(200'000); // initializes maximum motor acceleration
  s1.setPosition(0);

}
//int main(){
  void loop(){
  while(Serial.read() != 'n') {
    // Pressure Transducer Reading
      //delayMicroseconds(dt - loopActual);
      delay(10);
      time_start = micros();

      // measure pressure from ptin
      pt_analog_readingin = analogRead(ptin_pin); //reads value from input pin and assigns to variable
      pt_digital_readingin = (pt_analog_readingin - ptin_analog_zero)/(ptin_analog_max - ptin_analog_zero)*ptin_psi_max; //conversion equation to convert analog reading to psi

      // measure pressure from pt1
      pt_analog_reading1 = analogRead(pt1_pin); //reads value from input pin and assigns to variable
      pt_digital_reading1 = (pt_analog_reading1 - pt1_analog_zero)/(pt1_analog_max - pt1_analog_zero)*pt1_psi_max; //conversion equation to convert analog reading to psi
      if (sensorDataFilteredOld == 0){
        sensorDataFilteredOld = pt_digital_reading1;
      }

      //prints pressure readings to serial
      Serial.print("Input Pressure Transducer: ");
      Serial.print(pt_analog_readingin*3.3/1024);
      Serial.print("V     ");
      Serial.print(pt_digital_readingin, 1);
      Serial.println("psi");
      Serial.print("Output Pressure Transducer: ");
      Serial.print(pt_analog_reading1*3.3/1024);
      Serial.print("V     "); //prints label to serial
      Serial.print(pt_digital_reading1, 1); //prints value from previous line to serial
      Serial.println("psi"); //prints label to serial
      

      // end measurement time recording
      //time_stop = micros();
      //time_pc = time_stop - time_start;
      

    // PID System
      // Low Pass Noise Filter Exponential Moving Average (EMA)
      /*sensorDataRaw = pt_digital_reading1;
      sensorDataFiltered = smoothingFactor * sensorDataRaw + (1 - smoothingFactor) * sensorDataFilteredOld;
      sensorDataFilteredOld = sensorDataFiltered;*/
      SUM = SUM - READINGS[INDEX];       // Remove the oldest entry from the sum
      READINGS[INDEX] = pt_digital_reading1;           // Add the newest reading to the window
      SUM = SUM + pt_digital_reading1;                 // Add the newest reading to the sum
      INDEX = (INDEX+1) % WINDOW_SIZE;   // Increment the index, and wrap to 0 if it exceeds the window size

      sensorDataFiltered = SUM / WINDOW_SIZE;      // Divide the sum of the window by the window size for the result


      Serial.println(sensorDataFiltered);

      // PID
      actual = sensorDataFiltered;
      errorOld = error;
      error = target - actual;
      errorChange = error - errorOld;
      errorDerivative = errorChange / dt;
      errorIntegral = errorIntegral + (error * dt);
      actuatorSignal = -0.1 * ((Kp * error) + (Ki * errorIntegral) + (Kd * errorDerivative));

      // Anti-Winding Clamping
      if (abs(actuatorSignal * 1000000 / dt) > motorVelocity){ // if desired speed is greater than motor speed
        if (error * actuatorSignal < 0){ // ASK MICHAEL BEFORE TEST ABOUT THIS INEQUALITY
          actuatorSignal = -0.1 * ((Kp * error) + (Kd * errorDerivative));
           Serial.println("anit winding");
          }
        }

      // Stepper Motor Position Security
      if (s1.getPosition() == actuatorSaturationClosed){ // fully closed valve state
        if (actuatorSignal > 0){ // if the valve 'needs' to close more than fully closed
          actuatorSignal = 0;
        }
      }
      else if (s1.getPosition() == actuatorSaturationOpen){ // fully open valve state
        if (actuatorSignal < 0){ // if the valve 'needs' to open more than fully open
          actuatorSignal = 0;
        }
      }

      // Anti-Winding Back Calculation
      // if (abs(actuatorSignal) > actuatorSaturation){
      //   saturationReduction = actuatorSaturation - abs(actuatorSignal)
      //   actuatorSignal = (Kp * error)+ (Ki * errorIntegral + Kb * saturationReduction) + (Kd * errorDerivative) + bias;
      //   }
      Serial.println(actuatorSignal);
    // Stepper Motor Control
      s1.moveRelAsync(actuatorSignal);
      
      
      time_stop = micros();
      loopActual = time_stop - time_start;
      
  }
  s1.setAcceleration(5000);
  s1.setMaxSpeed(200);
  s1.moveAbs(0);
  s1.setAcceleration(200'000);
  s1.setMaxSpeed(200'000);
  Serial.println("Valve Returned To Closed State");
  Serial.println("Process Ended");
  delay(10000);
}

