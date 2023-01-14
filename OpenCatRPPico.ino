#include <servo.pio.h>

//modify the model and board definitions
//***********************
#define BITTLE    //Petoi 9 DOF robot dog: 1 on head + 8 on leg
//#define NYBBLE  //Petoi 11 DOF robot cat: 2 on head + 1 on tail + 8 on leg
//#define CUB

//#define BiBoard     //ESP32 Board with 12 channels of built-in PWM for joints
//#define BiBoard2  //ESP32 Board with 16 channels of PCA9685 PWM for joints
#define DiySetup     //RP Pico Board connected to a PCA9685 PWM for joints
//***********************

//Send 'R' token to reset the birthmark in the EEPROM so that the robot will restart to reset
//#define AUTO_INIT  //activate it to automatically reset joint and imu calibration without prompts

//you can also activate the following modes (they will diable the gyro to save programming space)
//allowed combinations: RANDOM_MIND + ULTRASONIC, RANDOM_MIND, ULTRASONIC, VOICE, CAMERA
//#define ULTRASONIC      //for Nybble's ultrasonic sensor
//#define VOICE           //for LD3320 module
//#define CAMERA          //for Mu Vision camera

#include "src/OpenCat.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);//USB serial
  Serial.setTimeout(5);
  //  Serial1.begin(115200); //second serial port
  while (Serial.available() && Serial.read()) {
    delay(1);
  }; // empty buffer
#ifdef VOLTAGE
  while (lowBattery());
#endif

  lastCmd[0] = '\0';
  newCmd[0] = '\0';
  initRobot();//moved all the init stuff to the header
}

void loop() {
#ifdef MAIN_SKETCH
#ifdef VOLTAGE
  lowBattery();//  block the loop if battery is low
  //  can be disabled to save programming space and reduce the low voltage interruptions
#endif
  //  //—self-initiative
  //  if (autoSwitch) { //the switch can be toggled on/off by the 'z' token
  //    randomMind();//allow the robot to auto rest and do random stuff in randomMind.h
  //    powerSaver(POWER_SAVER);//make the robot rest after a certain period, the unit is seconds
  //
  //  }
  //  //— read environment sensors (low level)
#ifdef GYRO_PIN
  readEnvironment();     //reads the IMU (Inertia Measurement Unit, i.e. acceleration and angles).
                         //May read more sensors in the future
  dealWithExceptions();  //fall over, lifted, etc.
#endif
  readSignal();
  //  readHuman();

reaction();  //handle different commands
#ifdef NEOPIXEL_PIN
  playLight();
#endif
#ifdef
  calibratePCA9685();
#endif
#endif
}
