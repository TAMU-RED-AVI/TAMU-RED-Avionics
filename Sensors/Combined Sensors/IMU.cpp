#include <Arduino.h>
#include <Wire.h>
#include "LSM6DSL.h"
#include "LIS3MDL.h"


int BerryIMUversion = 99;

void writeTo(int device, byte address, byte val) {
   Wire1.beginTransmission(device); //start transmission to device 
   Wire1.write(address);        // send register address
   Wire1.write(val);        // send value to write
   Wire1.endTransmission(); //end transmission
}


void readFrom(int device, byte address, int num, byte buff[]) {
  Wire1.beginTransmission(device); //start transmission to device 
  Wire1.write(address);        //sends address to read from
  Wire1.endTransmission(); //end transmission
  
  Wire1.beginTransmission(device); //start transmission to device (initiate again)
  Wire1.requestFrom(device, num);    // request 6 bytes from device
  
  int i = 0;
  while(Wire1.available())    //device may send less than requested (abnormal)
  { 
    buff[i] = Wire1.read(); // receive a byte
    i++;
  }
  Wire1.endTransmission(); //end transmission
}



void detectIMU(){
  //Detect which version of BerryIMU is connected using the 'who am i' register
  //BerryIMUv3 uses the LSM6DSL and LIS3MDL
  
  Wire1.begin(); 
  byte LSM6DSL_WHO_AM_I_response;
  byte LIS3MDL_WHO_AM_I_response;
 
  byte WHOresponse[2];
  
  //Detect if BerryIMUv3 (Which uses the LSM6DSL and LIS3MDL) is connected
  readFrom(LSM6DSL_ADDRESS, LSM6DSL_WHO_AM_I,1,WHOresponse);
  LSM6DSL_WHO_AM_I_response = WHOresponse[0];
  
  readFrom(LIS3MDL_ADDRESS, LIS3MDL_WHO_AM_I,1,WHOresponse);
  LIS3MDL_WHO_AM_I_response = WHOresponse[0];

  if (LSM6DSL_WHO_AM_I_response == 0x6A && LIS3MDL_WHO_AM_I_response == 0x3D){
    Serial.println("IMU Responding...");
    BerryIMUversion = 3; 
  }
}

void enableIMU(){
    //initialise the accelerometer
    writeTo(LSM6DSL_ADDRESS,LSM6DSL_CTRL1_XL,0b10011111);        // ODR 3.33 kHz, +/- 8g , BW = 400hz
    writeTo(LSM6DSL_ADDRESS,LSM6DSL_CTRL8_XL,0b11001000);        // Low pass filter enabled, BW9, composite filter
    writeTo(LSM6DSL_ADDRESS,LSM6DSL_CTRL3_C,0b01000100);         // Enable Block Data update, increment during multi byte read
   
}

void readACC(byte buff[]){
   readFrom(LSM6DSL_ADDRESS, LSM6DSL_OUT_X_L_XL, 6, buff);
} 
