// import libaries
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"

// defining I2C addresses for Multiplexer and Thermocouple Amplifier
#define TCA9548A_ADDRESS 0x70
#define MCP9600_ADDRESS 0x67

// number of thermocouple amplifiers
#define num_MCP9600 8

// needed for mcp commands
Adafruit_MCP9600 mcp;

// initialize multiplexer I2C ports
void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCA9548A_ADDRESS);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

// initialize thermocouple amplifiers
void setup(void) {

  // start recording time for initialization
  unsigned long time_start;
  unsigned long time_stop;
  time_start = micros();

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

  // record total time to initialize sensors
  time_stop = micros();
  Serial.print("Time to Initialize Sensors (μs): "); Serial.print(time_stop - time_start); Serial.println('\n');

  // not sure what mcp.enable() does
  mcp.enable(true);
  Serial.println(F("------------------------------"));
}

// measure temperature indefinitely 
void loop(void) { 
  // Start recording time for temp readings
  unsigned long time_start;
  unsigned long time_stop;
  time_start = micros();

  // read all sensors temps
  for (int i = 1; i <= num_MCP9600; ++i) {
    tcaselect(i);
    Serial.print("TC "); Serial.println(i);
    Serial.print("Hot Junction (C):        "); Serial.println(mcp.readThermocouple());
    Serial.print("Cold Junction (C):       "); Serial.println(mcp.readAmbient());
  }

  // record total time to measure temp
  time_stop = micros();
  Serial.print("\nMeasurement Time (μs):   "); Serial.print(time_stop - time_start); Serial.print('\n');
  Serial.println(F("------------------------------"));
  delay(1000);
}
