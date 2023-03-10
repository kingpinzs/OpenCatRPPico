/* BiBoard
    PWM:
                        |--------------------------------
                        |    PWM[0]           PWM[6]    |
                        |    PWM[1]           PWM[7]    |
                        |    PWM[2]           PWM[8]    |
                        |-----------                    |
                        |           |                   |
                        |   ESP32   |  IMU         USB-C|~~~~Tail~~~~
                        |           |                   |
                        |-----------                    |
                        |    PWM[3]           PWM[9]    |
                        |    PWM[4]           PWM[10]   |
                        |    PWM[5]           PWM[11]   |
                        |-------------------------------|

    Pin Name    |   ESP32 Pin   | RBP Pico Pin Name | Arduino Pin Name    |   Alternative Function
    PWM[0]          GPIO4                                    4                   GPIO / Ain / Touch
    PWM[1]          GPIO5                                    5                   GPIO / VSPI SS
    PWM[2]          GPIO18                                   18                  GPIO / VSPI SCK
    -------------------------------------------------------------------------------------------------
    PWM[3]          GPIO32                                   32                  GPIO / Ain / Touch
    PWM[4]          GPIO33                                   33                  GPIO / Ain / Touch
    PWM[5]          GPIO19                                   19                  GPIO / VSPI MISO
    --------------------------------------------------------------------------------------------------
    PWM[6]          GPIO2                                    2                   boot pin, DO NOT PUT HIGH WHEN BOOT!
    PWM[7]          GPIO15                                   15                  GPIO / HSPI SS / Ain Touch
    PWM[8]          GPIO13                                   13                  built-in LED / GPIO / HSPI MOSI / Ain / Touch
    --------------------------------------------------------------------------------------------------
    PWM[9]          GPIO12                                   12                  GPIO / HSPI MISO / Ain / Touch
    PWM[10]         GPIO14                                   14                  GPIO / HSPI SCK / Ain / Touch
    PWM[11]         GPIO27                                   27                  GPIO / Ain / Touch

    I2C:

    Pin Name    |   ESP32 Pin   | RBP Pico Pin Name |  Arduino Pin Name    |   Alternative Function
    I2C-SCL         GPIO22                                    22                  Fixed - ICM20600 - Pulled
    I2C-SDA         GPIO21                                    21                  Fixed = ICM20600 - Pulled

    System default, nothing to declaration!

    Other Peripherals:

    Pin Name    |   ESP32 Pin   | RBP Pico Pin Name |  Arduino Pin Name    |   Alternative Function
    IR_Remote       GPIO23                                   23                  Fixed - VS1838B IR
    DAC_Out         GPIO25                                   25                  Fixed - PAM8302
    IMU_Int         GPIO26                                   26                  Fixed - MPU6050 Interrupt

    System default, nothing to declaration!
*/

/* BiBoard2
  IMU_Int     27
  BUZZER      14
  VOLTAGE     4
  RGB LED     15
  GREEN-LED   5
*/

/*  Total DOF            Walking DOF
                   Nybble    Bittle    Cub
   BiBoard  (12)  skip 0~4  skip 0~4    12
   BiBoard2 (16)  skip 0~8  skip 0~8  skip0~4
*/

/*
The INTERRUPT_PIN is what ever you set it to be

We define a pin as being the ???interrupt input???, and we define what change of state on that point is considered to be an interrupt. On the Pico, we can use any GPIO pin for this, and we can define more than one.
We create an ???interrupt handler??? function, something we want to run when an interrupt is detected.
We pair that ???interrupt handler??? with the ???interrupt input???.

*/
#define PICO
#define INTERRUPT_PIN 26
#define GYRO_PIN_SCL 4
#define GYRO_PIN_SDA 5
#define PWM_NUM 12
//L:Left-R:Right-F:Front-B:Back---LF, RF, RB, LB
const uint8_t PWM_pin[PWM_NUM] = {19,  4,  2, 27,   //head or shoulder roll
                                  33,  5, 15, 14,   //shoulder pitch
                                  32, 18, 13, 12    //knee
                                 };
#define SERVO_FREQ 240 //test frequincy between 200 and 300 to see what prefomce the best
#define VOLTAGE 4
#define LOW_VOLTAGE 6.8
#define VOLTAGE_DETECTION_PIN A0  //voltage detector
#define DOF 16
#define WALKING_DOF 8
#define GAIT_ARRAY_DOF 8

enum ServoModel_t {
  G41 = 0,
  P1S,
  P2K,
  HS55,
};



//Tutorial: https://bittle.petoi.com/11-tutorial-on-creating-new-skills
#ifdef NYBBLE
#define HEAD
#define TAIL
#define X_LEG
#define REGULAR G41
#define KNEE G41
#include "InstinctNybbleESP.h"

#elif defined BITTLE
#define HEAD
#define LL_LEG
#define REGULAR P1S
#define KNEE P1S
#include "InstinctBittleESP.h"

#elif defined CUB
#ifdef BiBoard2
#define HEAD
#define TAIL
#endif
#define LL_LEG
#define REGULAR P1S
#define KNEE P2K
#include "InstinctCubESP.h"
//#define MPU_YAW180
#endif

ServoModel_t servoModelList[] = {
  REGULAR, REGULAR, REGULAR, REGULAR,
  REGULAR, REGULAR, REGULAR, REGULAR,
  REGULAR, REGULAR, REGULAR, REGULAR,
  KNEE, KNEE, KNEE, KNEE
};

#include <math.h>
//token list
#define T_ABORT       'a'
#define T_BEEP        'b'
#define T_BEEP_BIN    'B'
#define T_CALIBRATE   'c'
#define T_COLOR       'C'
#define T_REST        'd'
#define T_GYRO        'g'
#define T_HELP        'h'
#define T_INDEXED_SIMULTANEOUS_ASC  'i'
#define T_INDEXED_SIMULTANEOUS_BIN  'I'
#define T_INDEXED_SEQUENTIAL_BIN 'M'
#define T_JOINTS      'j'
#define T_SKILL       'k'
#define T_SKILL_DATA  'K'
#define T_LISTED_BIN  'L'
#define T_MOVE_ASC    'm'
#define T_MOVE_BIN    'M'
#define T_MELODY      'o'
#define T_PAUSE       'p'
#define T_RAMP        'r'
#define T_RESET       'R'
#define T_SAVE        's'
#define T_SOUND       'S'
#define T_TILT        't'
#define T_TEMP        'T'           //call the last skill data received from the serial port
#define T_MEOW        'u'
#define T_PRINT_GYRO            'v' //print Gyro data
#define T_VERBOSELY_PRINT_GYRO  'V' //verbosely print Gyro data
#define T_WORD        'w'
#define T_SERVO_MICROSECOND 'W'   //PWM width modulation
#define T_XLEG        'x'
#define T_ACCELERATE  '.'
#define T_DECELERATE  ','
#define T_RANDOM_MIND 'z'

bool updated[10];
float degPerRad = 180 / M_PI;
float radPerDeg = M_PI / 180;

//control related variables
#define POWER_SAVER 120 //make the robot rest after a certain period, unit is seconds
#define IDLE_SHORT 15
#define IDLE_LONG 30
#define EVERY_X_SECONDS 5
int idleThreshold = IDLE_SHORT;
#define RANDOM_MIND true     //let the robot do random stuffs. use token 'z' to activate/deactivate

#define IDLE_TIME 5000
int randomMindListLength;
int randomBase = 0;
long randTimer;
long idleTimer;
int randomInterval = 1000;
#define CHECK_BATTERY_PERIOD 10000  //every 10 seconds. 60 mins -> 3600 seconds
int uptime = -1;
int frame;
int tStep = 1;
char token;
char lastToken;
#define CMD_LEN 10  //the last char will be '\0' so only CMD_LEN-1 elements are allowed 
char *newCmd = new char[CMD_LEN];
char *lastCmd = new char[CMD_LEN];
int cmdLen;
byte newCmdIdx = 0;
int8_t* dataBuffer = new int8_t[1524];
int8_t yprTilt[3];
int lastVoltage;
bool servoOff = true;


bool checkGyro = false;
bool printGyro = false;
bool walkingQ = false;
bool imuUpdated;
byte exceptions = 0;
byte transformSpeed = 2;
float protectiveShift;//reduce the wearing of the potentiometer 


bool initialBoot = true;
bool safeRest = true;

int delayLong = 20;
int delayMid = 8;
int delayShort = 2;
int delayStep = 1;
int runDelay = delayMid;

#ifdef RANDOM_MIND
bool autoSwitch = true;
#endif



#ifdef NYBBLE
int8_t middleShift[] = {0, 15, 0, 0,
                        -45, -45, -45, -45,
                        10, 10, -10, -10,
                        -30, -30, 30, 30
                       };
#elif defined BITTLE
int8_t middleShift[] = {0, 15, 0, 0,
                        -45, -45, -45, -45,
                        55, 55, -55, -55,
                        -55, -55, -55, -55
                       };

#else //CUB
int8_t middleShift[] = {0, 15, 0, 0,
                        -45, -45, -45, -45,
                        55, 55, -55, -55,
                        -45, -45, -45, -45
                       };
#endif

#ifdef CUB
int8_t rotationDirection[] = {1, -1, 1, 1,
                              1, -1, 1, -1,
                              1, -1, -1, 1,
                              1, -1, -1, 1
                             };
int angleLimit[][2] = {
  { -120, 120}, { -30, 80}, { -120, 120}, { -120, 120},
  { -90, 60}, { -90, 60}, { -90, 90}, { -90, 90},
  { -180, 120}, { -180, 120}, { -80, 200}, { -80, 200},
  { -66, 100}, { -66, 100}, { -66, 100}, { -66, 100},
};
#else
int8_t rotationDirection[] = {1, -1, 1, 1,
                              1, -1, 1, -1,
                              1, -1, -1, 1,
                              -1, 1, 1, -1
                             };
int angleLimit[][2] = {
  { -120, 120}, { -30, 80}, { -120, 120}, { -120, 120},
  { -90, 60}, { -90, 60}, { -90, 90}, { -90, 90},
  { -200, 80}, { -200, 80}, { -80, 200}, { -80, 200},
  { -80, 200}, { -80, 200}, { -70, 200}, { -80, 200},
};
#endif

#ifdef X_LEG
int currentAng[DOF] = { -30, -80, -45, 0,
                        0, 0, 0, 0,
                        75,  75,  -75,  -75,
                        -55, -55, 55, 55
                      };
int previousAng[DOF]= { -30, -80, -45, 0,
                        0, 0, 0, 0,
                        75,  75,  -75,  -75,
                        -55, -55, 55, 55
                      };
#else
int currentAng[DOF] = { -30, -80, -45, 0,
                        0, 0, 0, 0,
                        75,  75,  75,  75,
                        -55, -55, -55, -55
                      };
int previousAng[DOF]= {  -30, -80, -45, 0,
                        0, 0, 0, 0,
                        75,  75,  75,  75,
                        -55, -55, -55, -55
                      };                    
#endif
int zeroPosition[DOF] = {};
int calibratedZeroPosition[DOF] = {};

int8_t servoCalib[DOF] = {0, 0, 0, 0,
                          0, 0, 0, 0,
                          0, 0, 0, 0,
                          0, 0, 0, 0
                         };


//abbreviations
#define PT(s) Serial.print(s)  //makes life easier
#define PTL(s) Serial.println(s)
#define PTF(s) Serial.print(F(s))//trade flash memory for dynamic memory with F() function
#define PTLF(s) Serial.println(F(s))
#define PRINT_SKILL_DATA

//
//template <typename T> int8_t sign(T val) {
//  return (T(0) < val) - (val < T(0));
//}

void printRange(int r0 = 0, int r1 = 0) {
  if (r1 == 0)
    for (byte i = 0; i < r0; i++) {
      PT(i);
      PT('\t');
    }
  else
    for (byte i = r0; i < r1; i++) {
      PT(i);
      PT('\t');
    }
  PTL();
}

String range2String(int r0 = 0, int r1 = 0) {
  String temp = "";
  if (r1 == 0)
    for (byte i = 0; i < r0; i++) {
      temp += i;
      temp += '\t';
    }
  else
    for (byte i = r0; i < r1; i++) {
      temp += i;
      temp += '\t';
    }
  return temp;
}

template<typename T> void printList(T *arr, byte len = DOF) {
  String temp = "";
  for (byte i = 0; i < len; i++) {
    temp += String(int(arr[i]));
    temp += ",\t";
    //PT((T)(arr[i]));
    //PT('\t');
  }
  PTL(temp);
}

template<typename T> String list2String(T *arr, byte len = DOF) {
  String temp = "";
  for (byte i = 0; i < len; i++) {
    temp += String(int(arr[i]));  //String(int(arr[i]));
    temp += ",\t";
    //PT((T)(arr[i]));
    //PT('\t');
  }
  return temp;
}

//float sign(float value) {
//  return ((value > 0) - (value < 0));
//}
template<typename T> void printTable(T *list) {
  printRange(0, DOF);
  printList(list, DOF);
}
