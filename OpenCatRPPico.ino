#include <servo.pio.h>
#include "src/OpenCat.h"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200); // USB serial
  Serial.setTimeout(5);
  //  Serial1.begin(115200); //second serial port
  while (Serial.available() && Serial.read())
  {
    delay(1);
  }; // empty buffer

  while (lowBattery());

  lastCmd[0] = '\0';
  newCmd[0] = '\0';
  initRobot(); // moved all the init stuff to the header
}

void loop()
{
#ifdef MAIN_SKETCH

  lowBattery(); //  block the loop if battery is low
  //  can be disabled to save programming space and reduce the low voltage interruptions

  //  //—self-initiative
  //  if (autoSwitch) { //the switch can be toggled on/off by the 'z' token
  //    randomMind();//allow the robot to auto rest and do random stuff in randomMind.h
  //    powerSaver(POWER_SAVER);//make the robot rest after a certain period, the unit is seconds
  //
  //  }
  //  //— read environment sensors (low level)
#ifdef GYRO_PIN
  readEnvironment(); // reads the IMU (Inertia Measurement Unit, i.e. acceleration and angles).
                     // May read more sensors in the future
  dealWithExceptions(); // fall over, lifted, etc.
#endif
  readSignal();
  //  readHuman();

  reaction(); // handle different commands
  calibratePCA9685();

#endif
}
