#include "IMU.h"
#include "LSM6DSL.h"
#include "LIS3MDL.h"


byte buff[6];
int accRaw[3];
float accx, accy, accz;

void setup() {
  Serial.begin(1000000);  // start serial for output
  Wire.setClock(1000000);
  // put your setup code here, to run once:
  detectIMU();
  Serial.println("IMU Detected...");
  enableIMU();
  Serial.println("IMU Enabled...");
}

void loop() {
  // put your main code here, to run repeatedly:
  readACC(buff);
  accRaw[zero] = (int16_t)(buff[0] | (buff[1] << 8)); 
  accRaw[one] = (int16_t)(buff[2] | (buff[3] << 8));
  accRaw[two] = (int16_t)(buff[4] | (buff[5] << 8));

  accx = accRaw[0]/accoffset;
  accy = accRaw[1]/accoffset;
  accz = accRaw[2]/accoffset;

  Serial.print(" ### acc x>> ");
  Serial.print(accx);
  Serial.print(" ### acc y>> ");
  Serial.print(accy);
  Serial.print(" ### acc z>> ");
  Serial.print(accz);
  Serial.println();
}
