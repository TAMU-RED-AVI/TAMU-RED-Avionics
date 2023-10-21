
/* this code is specifically for a Teensy conencted to 2 pressure transducers and an I2C multiplexer which has thermocouple amplifiers connected to it.
*/

/* 
TIME MEASUREMENT VARABLES
-------------------------
*/

// time measurement variavles
unsigned long time_start;
unsigned long time_stop;
unsigned long time_tc;
unsigned long time_pc;
unsigned long time_lc;
unsigned long time_ac;

unsigned long lastUpdateTC;
unsigned long lastUpdatePC;
unsigned long lastUpdateLC;
unsigned long lastUpdateAC;

int updateIntervalTC = 1000000;
int updateIntervalPC = 5000;
int updateIntervalLC = 12500;
int updateIntervalAC = 5000;

/* 
THERMOCOUPLE IMPORTS, VARIBLE SETUP, ETC
----------------------------------------
*/

// import libaries
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"

// defining I2C addresses for Multiplexer and Thermocouple Amplifier
#define TCA9548A_ADDRESS 0x70
#define MCP9600_ADDRESS 0x67

// thermocouple amplifier setup
#define num_MCP9600 8 //number of thermocouples for the multiplexer [INPUT VARIABLE]
Adafruit_MCP9600 mcp; // needed for mcp commands 

float tc_hot;

// initialize multiplexer I2C ports
void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCA9548A_ADDRESS);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

/* 
PRESSURE TRANSDUCER VARIBLE SETUP
---------------------------------
*/

// pin setup
const int pt1_pin = A6; //select the analog input pin for pressure transducer 1
const int pt2_pin = A7; //select the analog input pin for pressure transducer 2

// 300 psi PT values
const int pt1_analog_zero = 102.4; //analog reading of pressure transducer at 0psi
const int pt1_analog_max = 921.6; //analog reading of pressure transducer at 100psi
const int pt1_psi_max = 300; //max psi value of transducer being used

// 500 psi PT values (NEEDS TO BE UPDATED)
const int pt2_analog_zero = 102.4; //analog reading of pressure transducer at 0psi
const int pt2_analog_max = 921.6; //analog reading of pressure transducer at 100psi
const int pt2_psi_max = 300; //max psi value of transducer being used

// temp reading variables
float pt_analog_reading1 = 0; //variable to store the value coming from the pressure transducer
float pt_digital_reading1 = 0; //variable to store the value coming from the pressure transducer
float pt_analog_reading2 = 0; //variable to store the value coming from the pressure transducer
float pt_digital_reading2 = 0; //variable to store the value coming from the pressure transducer


/* 
ACCELEROMETER SETUP
---------------------------------
*/

// custom libraries
#include "IMU.h"
#include "LSM6DSL.h"
#include "LIS3MDL.h"

// variable setup
byte buff[6];
int accRaw[3];
float accx, accy, accz;
const float accoffset = 4180;

/*
LOAD CELL SET UP
------------------------------
*/

// library
#include "HX711.h"

// load cell 1
const int LOADCELL_DOUT_PIN = 5;
const int LOADCELL_SCK_PIN = 4;

// load cell 2
// const int LOADCELL_DOUT_PIN2 = 6;
// const int LOADCELL_SCK_PIN2 = 7;

float weight1;
float weight2;
HX711 scale, scale2;

unsigned long loopStartTime; // Variable to store the start time of the loop

/* 
VOID SETUP FUNCTION
---------------------------------------
*/

// initialize thermocouple amplifiers
void setup(void) {

  /* 
  THERMOCOUPLE SET UP
  -------------------
  */

  // begin I2C master transmission
  Serial.begin(115200);
  Wire.begin();

  // wait for serial port to connect
  while (!Serial) {
    delay(10);
  }

  // detect sensors
  Serial.println("MCP9600 Initialization:");
  for (int i = 1; i <= num_MCP9600; ++i) {
    tcaselect(i);
    if (! mcp.begin(MCP9600_ADDRESS)) {
          Serial.print("MCP9600 "); Serial.print(i); Serial.println(" not found. Check wiring!");

          // stops code if sensor not found
          while (1);
    }
    else;
      Serial.print("MCP9600 "); Serial.print(i); Serial.println(" found!");
  }
  
  // set ADC resolution
  mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
  Serial.print("ADC resolution set to ");
  switch (mcp.getADCresolution()) {
    case MCP9600_ADCRESOLUTION_18:   Serial.print("18"); break;
    case MCP9600_ADCRESOLUTION_16:   Serial.print("16"); break;
    case MCP9600_ADCRESOLUTION_14:   Serial.print("14"); break;
    case MCP9600_ADCRESOLUTION_12:   Serial.print("12"); break;
  }
  Serial.println(" bits");

  // set thermocouple type
  mcp.setThermocoupleType(MCP9600_TYPE_K);
  Serial.print("Thermocouple type set to ");
  switch (mcp.getThermocoupleType()) {
    case MCP9600_TYPE_K:  Serial.print("K"); break;
    case MCP9600_TYPE_J:  Serial.print("J"); break;
    case MCP9600_TYPE_T:  Serial.print("T"); break;
    case MCP9600_TYPE_N:  Serial.print("N"); break;
    case MCP9600_TYPE_S:  Serial.print("S"); break;
    case MCP9600_TYPE_E:  Serial.print("E"); break;
    case MCP9600_TYPE_B:  Serial.print("B"); break;
    case MCP9600_TYPE_R:  Serial.print("R"); break;
  }
  Serial.println(" type");

  // set filter coefficient (not sure what this is)
  mcp.setFilterCoefficient(3);
  Serial.print("Filter coefficient value set to: ");
  Serial.println(mcp.getFilterCoefficient());

  // set alert temperature
  mcp.setAlertTemperature(1, 30);
  Serial.print("Alert #1 temperature set to ");
  Serial.println(mcp.getAlertTemperature(1));
  mcp.configureAlert(1, true, true);  // alert 1 enabled, rising temp

  // not sure what mcp.enable() does
  mcp.enable(true);

  /* 
  ACCELEROMETER SET-UP
  --------------------
  */

  Serial.begin(1000000);  // start serial for output
  Wire1.setClock(1000000);
  // put your setup code here, to run once:
  detectIMU();
  Serial.println("IMU Detected...");
  enableIMU();
  Serial.println("IMU Enabled...");

  /*
  LOAD CELL SET UP
  -----------------------
  */

  Serial.begin(500000);

  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  // scale2.begin(LOADCELL_DOUT_PIN2, LOADCELL_SCK_PIN2);

  Serial.println("Taring the scale");
  scale.set_scale(2280.f);  // Set the scale factor for conversion to kilograms
  scale.tare();             // Reset the scale to zero

  // scale2.set_scale(2280.f); // Set the scale factor for conversion to kilograms
  // scale2.tare();            // Reset the scale to zero

  Serial.println("After taring the scale:");

  Serial.println(F("------------------------------"));
}

/* 
MEASUREMENT LOOP
-------------------------------------------
*/
void loop(void) { 
  
  if((micros() - lastUpdateTC) > updateIntervalTC){
  lastUpdateTC = micros();

  /*
  THERMOCOUPLE READINGS
  ---------------------
  */

  // start recording temp time
  time_start = micros();

  // read all sensors temps
  for (int i = 1; i <= num_MCP9600; ++i) {
    tcaselect(i - 1);
    tc_hot = mcp.readThermocouple();
    Serial.print("TC "); Serial.print(i); Serial.print(" "); Serial.print("Temperature (C):        "); Serial.println(tc_hot); // measure & print hot junction temp 
  // Serial.print("Cold Temp (C):       "); Serial.println(mcp.readAmbient()); // cold junction temp
  }

  // record total time to measure temp
  time_stop = micros();
  time_tc = time_stop - time_start;
  }

  /*
  PRESSURE TRANSDUCER READINGS
  ----------------------------
  */

  if((micros() - lastUpdatePC) > updateIntervalPC){
  lastUpdatePC = micros();

  // start recording pressure time
  time_start = micros();

  // measure pressure from pt1
  pt_analog_reading1 = analogRead(pt1_pin); //reads value from input pin and assigns to variable
  pt_digital_reading1 = (pt_analog_reading1 - pt1_analog_zero)/(pt1_analog_max - pt1_analog_zero)*pt1_psi_max/.66; //conversion equation to convert analog reading to psi

  // measure pressure from pt2
  pt_analog_reading2 = analogRead(pt2_pin); //reads value from input pin and assigns to variable
  pt_digital_reading2 = (pt_analog_reading2 - pt2_analog_zero)/(pt2_analog_max - pt2_analog_zero)*pt2_psi_max/.66; //conversion equation to convert analog reading to psi

  // record total time to measure pressure
  time_stop = micros();
  time_pc = time_stop - time_start;
  }

  /*
  ACCELEROMETER READINGS
  ----------------------------
  */

  if((micros() - lastUpdateAC) > updateIntervalAC){
  lastUpdateAC = micros();

  time_start = micros();

  readACC(buff);
  accRaw[0] = (int16_t)(buff[0] | (buff[1] << 8)); 
  accRaw[1] = (int16_t)(buff[2] | (buff[3] << 8));
  accRaw[2] = (int16_t)(buff[4] | (buff[5] << 8));

  accx = accRaw[0]/accoffset;
  accy = accRaw[1]/accoffset;
  accz = accRaw[2]/accoffset;

  time_stop = micros();
  time_ac = time_stop - time_start;
  }

  /*
  LOAD CELL READINGS
  -------------------------------
  */


  if((micros() - lastUpdateLC) > updateIntervalLC){
  lastUpdateLC = micros();
  
  loopStartTime = micros(); // Record the start time of the loop

  // Reading the weight from both load cells
  weight1 = scale.get_units(1);  // Get the weight in kilograms
  weight2 = scale2.get_units(1); // Get the weight in kilograms

  // Calculate and print the loop duration
  unsigned long loopEndTime = micros(); // Record the end time of the loop
  unsigned long time_lc = loopEndTime - loopStartTime;
  }

  /*
  SERIAL PRINTING READINGS
  */

  Serial.print("\nPT 1 Voltage  (mV):          "); Serial.println(pt_analog_reading1, 1); //prints calculated psi value
  Serial.print("PT 1 Pressure (psi):         "); Serial.println(pt_digital_reading1, 1); //prints calculated psi value
  Serial.print("PT 2 Voltage  (mV):          "); Serial.println(pt_analog_reading2, 1); //prints calculated psi value
  Serial.print("PT 2 Pressure (psi):         "); Serial.println(pt_digital_reading2, 1); //prints calculated psi value
  Serial.println();
  Serial.print("Acceleration (x, y, z) (g): "); Serial.print(accx); Serial.print(", "); Serial.print(accy); Serial.print(", "); Serial.print(accz); 
  Serial.println();
  Serial.print("LC 1 Force (kg):           ");
  Serial.println(weight1, 1);
  Serial.print("LC 2 Force (kg):           ");
  Serial.println(weight2, 1);

  /*
  MEASUREMENT TIME READINGS 
  -------------------------
  */

  Serial.print("\nTC Measurement Time (μs):     "); Serial.println(time_tc);
  Serial.print("PC Measurement Time (μs):       "); Serial.println(time_pc);
  Serial.print("AC Measurement Time (μs):       "); Serial.println(time_ac);
  Serial.print("LC Measurement Time (μs):       "); Serial.println(time_lc);

  Serial.println(F("----------------------------------"));
}
