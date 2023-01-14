#include "OpenCatSettings.h" //move all the define and setting code here

#include <EEPROM.h>

#include "sound.h"
//#include "I2cEEPROM.h"



#ifdef VOICE
#include "voice.h"
#endif
#ifdef CAMERA
#include "camera.h"
#endif
#ifdef RANDOM_MIND
#define ALL_RANDOM  // add random joint movements between the choice list of preset behaviors
#include "randomMind.h"
#endif
#include <hardware/watchdog.h>
//#include <avr/wdt.h> 
#include "eeprom.h"
#include "io.h"
#include "PCA9685servo.h"
#include "motion.h"
#include "skill.h"
#ifdef NEOPIXEL_PIN
#include "led.h"
#endif
#ifdef GYRO_PIN
#include "imu.h"
#endif
#ifdef IR_PIN
#include "infrared.h"
#endif

#ifdef ULTRASONIC
#include "ultrasonic.h"
#endif
#include "reaction.h"
#include "qualityAssurance.h"



void initRobot() { //move the inti stuff from main file to here
  //----------------------------------
#ifdef MAIN_SKETCH  // **
  PTLF("Flush the serial buffer...");
  PTLF("\n* Start *");

  while (Serial.available() && Serial.read()); // empty buffer
  i2cDetect();
  #ifdef I2C_EEPROM
    i2cEepromSetup();
  #endif

  PTL('k');
#ifdef GYRO_PIN
  imuSetup();
#endif

// not sure what these are used for yet
  bleSetup();
  blueSspSetup();


#ifdef IR_PIN
  irrecv.enableIRIn();  // Start the receiver
  gait.reserve(4);
#endif

// not sure which is correct yet in the LED section
//LED section
#ifdef PWM_LED_PIN
  pinMode(PWM_LED_PIN, OUTPUT);
#endif
#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
#endif
//end of LED section
#ifdef NEOPIXEL_PIN
  //  pixel.begin();           // INITIALIZE NeoPixel pixel object (REQUIRED)
  //  pixel.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  ledSetup();
#endif

  servoSetup();
  //skill.assignSkillAddressToOnboardEeprom();// ?????
  skillList = new SkillList();
#ifdef RANDOM_MIND
  for (byte i = 0; i < randomMindListLength; i++) {
    randomBase += choiceWeight[i];
  }
#endif
  randomSeed(analogRead(A2));  //use the fluctuation of voltage caused by servos as entropy pool

  QA();
  i2c_eeprom_write_byte( EEPROM_BIRTHMARK_ADDRESS, BIRTHMARK);//finish the test and mark the board as initialized

#ifdef VOICE
  voiceSetup();
#endif
#ifdef VOICE_LD3320
  voiceLD3320Setup();
#endif
#ifdef CAMERA
  cameraSetup();
#endif
#ifdef GESTURE
  gestureSetup();
#endif

  playMelody(MELODY_NORMAL);

  delay(2000);  //change the delay if the app doesn't recognize the Petoi device.
#ifdef GYRO_PIN
  for (byte r = 0; r < 3; r++)
    read_IMU();                                                             //ypr is slow when starting up. leave enough time between IMU initialization and this reading
  token = (fabs(ypr[1]) > 30 || fabs(ypr[2]) > 30) ? T_CALIBRATE : T_REST;  //put the robot's side on the table to enter calibration posture for attaching legs
  newCmdIdx = 2;
#endif
  PTLF("\n* Start *");
#ifdef BITTLE
  PTLF("Bittle");
#elif defined NYBBLE
  PTLF("Nybble");
#elif defined CUB
  PTLF("Cub");
#endif
  //----------------------------------
#else  // ** save parameters to device's static memory
  configureEEPROM();
  servoSetup();  //servo needs to be after configureEEPROM and before imuSetup
#ifdef GYRO_PIN
  imuSetup();
#endif
#endif  // **

  allCalibratedPWM(currentAng);  //soft boot for servos
  delay(500);
  PTLF("Ready!");
  idleTimer = millis();
  playMelody(melodyNormalBoot, sizeof(melodyNormalBoot) / 2);
#ifndef MAIN_SKETCH
  PCA9685CalibrationPrompt();
#endif
}