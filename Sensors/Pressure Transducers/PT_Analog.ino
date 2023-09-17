

const int pressureInput = A5; //select the analog input pin for the pressure transducer
const int pressureZero = 102.4; //analog reading of pressure transducer at 0psi
const int pressureMax = 921.6; //analog reading of pressure transducer at 100psi
const int pressuretransducermaxPSI = 300; //psi value of transducer being used
const int sensorreadDelay = 1; //constant integer to set the sensor read delay in milliseconds

float pressureValueAnalog = 0; //variable to store the value coming from the pressure transducer
float pressureValueDigital = 0; //variable to store the value coming from the pressure transducer

void setup() //setup routine, runs once when system turned on or reset
{
  Serial.begin(112500); //initializes serial communication at set baud rate bits per second
}

void loop() //loop routine runs over and over again forever
{
  pressureValueAnalog = analogRead(pressureInput); //reads value from input pin and assigns to variable
  pressureValueDigital = ((pressureValueAnalog-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero); //conversion equation to convert analog reading to psi
  Serial.print(pressureValueAnalog);
  Serial.print("V      "); //prints label to serial
  Serial.print(pressureValueDigital, 1); //prints value from previous line to serial
  Serial.println("psi"); //prints label to serial

  delay(sensorreadDelay); //delay in milliseconds between read values
}
