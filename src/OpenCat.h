#include "OpenCatSettings.h" //move all the define and setting code here

#include <hardware/watchdog.h>
// #include <avr/wdt.h>
#include "io.h"
#include "PCA9685servo.h"
#include "motion.h"
#include "imu.h"
#include "skill.h"
#include "ultrasonic.h"
#include "reaction.h"
// #include "qualityAssurance.h"

void initRobot()
{ // move the inti stuff from main file to here
  //----------------------------------
#ifdef MAIN_SKETCH // **
  PTLF("Flush the serial buffer...");
  PTLF("\n* Start *");
  while (Serial.available() && Serial.read()); // empty buffer
  i2cDetect();
  PTL('k');
  imuSetup();

  // not sure what these are used for yet
  servoSetup();
  skillList = new SkillList();

  for (byte r = 0; r < 3; r++)
    read_IMU();                                                            // ypr is slow when starting up. leave enough time between IMU initialization and this reading
  token = (fabs(ypr[1]) > 30 || fabs(ypr[2]) > 30) ? T_CALIBRATE : T_REST; // put the robot's side on the table to enter calibration posture for attaching legs
  newCmdIdx = 2;

  allCalibratedPWM(currentAng); // soft boot for servos
  delay(500);
  PTLF("Ready!");
  idleTimer = millis();
  // playMelody(MELODY_NORMAL);
#ifndef MAIN_SKETCH
  PCA9685CalibrationPrompt();
#endif
}