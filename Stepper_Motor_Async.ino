//Anything relating to LED in this program is a placeholder for other code running asynchronously during motor motion
#include "Arduino.h"
#include "teensystep4.h"
using namespace TS4;

Stepper s1(5, 4); //indicates pin for STEP and DIR, respectively

void setup()
{
    TS4::begin();

    s1.setMaxSpeed(300'000); //initializes maximum motor speed; 300,000 pulses/s is maximum
    s1.setAcceleration(500'000); //initializes maximum motor acceleration; 500,000 pulses/s^2 is maximum

    s1.moveAbs(1000); // moves motor to arbitrary starting position
    //pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    s1.moveRelAsync(-50000); //moves motor -50,000 steps from current position; step size determined by driver settings
    //digitalWrite(LED_BUILTIN, HIGH);
    //delay(1000);
    //digitalWrite(LED_BUILTIN, LOW);
    //delay(1000);
    //digitalWrite(LED_BUILTIN, HIGH);
    //delay(1000);
    //digitalWrite(LED_BUILTIN, LOW);
    delay(5000);

    s1.moveRelAsync(50000); //moves motor 50,000 steps from current position; step size determined by driver settings
    //digitalWrite(LED_BUILTIN, HIGH);
    //delay(1000);
    //digitalWrite(LED_BUILTIN, LOW);
    //delay(1000);
    //digitalWrite(LED_BUILTIN, HIGH);
    //delay(1000);
    //digitalWrite(LED_BUILTIN, LOW);
    delay(5000);
}

