/*
 * @Author: ylt
 * @Date: 2019-10-11 12:44:21
 * @LastEditors: ylt
 * @LastEditTime: 2019-10-29 16:38:54
 * @FilePath: \BROBOT_EVO2\Servos.ino
 */

// Servo and aux functions

//#include "Arduino.h"

// Default servo definitions
#define SERVO1_AUX_NEUTRO 1500 // Servo neutral position
#define SERVO1_MIN_PULSEWIDTH 500
#define SERVO1_MAX_PULSEWIDTH 2500
#define SERVO2_AUX_NEUTRO 1500 // Servo neutral position
#define SERVO2_MIN_PULSEWIDTH 500
#define SERVO2_MAX_PULSEWIDTH 2500


float BATT_VOLT_FACTOR = 81.84;
float BATT_VOLT_FIRST;
float BATT_VOLT_SECOND;
float BATT_VOLT_DOWN;
float BATT_MAX_VOLT = 8.4 - BATT_VOLT_DOWN;
float BATT_MIN_VOLT = 6.4 - BATT_VOLT_DOWN;

float battery;
float BATT_VOLT_ACTUALLY;

float BATT_VOLT_ACTUALLY_PER;

// Init servo on T4 timer. Output OC4B (Leonardo Pin10)
// We configure the Timer4 for 11 bits PWM (enhacend precision) and 16.3ms period (OK for most servos)
// Resolution: 8us per step (this is OK for servos, around 175 steps for typical servo)
void BROBOT_initServo()
{
  int temp;

  // Initialize Timer4 as Fast PWM
  TCCR4A = (1 << PWM4A) | (1 << PWM4B);
  TCCR4B = 0;
  TCCR4C = (1 << PWM4D);
  TCCR4D = 0;
  TCCR4E = (1 << ENHC4); // Enhaced -> 11 bits

  temp = 1500 >> 3;
  TC4H = temp >> 8;
  OCR4B = temp & 0xff;

  // Reset timer
  TC4H = 0;
  TCNT4 = 0;

  // Set TOP to 1023 (10 bit timer)
  TC4H = 3;
  OCR4C = 0xFF;

  // OC4A = PC7 (Pin13)  OC4B = PB6 (Pin10)   OC4D = PD7 (Pin6)
  // Set pins as outputs
  DDRB |= (1 << 6); // OC4B = PB6 (Pin10 on Leonardo board)
  DDRC |= (1 << 7); // OC4A = PC7 (Pin13 on Leonardo board)
  DDRD |= (1 << 7); // OC4D = PD7 (Pin6 on Leonardo board)

  //Enable OC4A and OC4B and OCR4D output
  TCCR4A |= (1 << COM4B1) | (1 << COM4A1);
  TCCR4C |= (1 << COM4D1);
  // set prescaler to 256 and enable timer    16Mhz/256/1024 = 61Hz (16.3ms)
  TCCR4B = (1 << CS43) | (1 << CS40);
}

void BROBOT_moveServo1(int pwm)
{
  pwm = constrain(pwm, SERVO1_MIN_PULSEWIDTH, SERVO1_MAX_PULSEWIDTH) >> 3; // Check max values and Resolution: 8us
  // 11 bits => 3 MSB bits on TC4H, LSB bits on OCR4B
  TC4H = pwm >> 8;
  OCR4B = pwm & 0xFF;
}

void BROBOT_moveServo2(int pwm)
{
  pwm = constrain(pwm, SERVO2_MIN_PULSEWIDTH, SERVO2_MAX_PULSEWIDTH) >> 3; // Check max values and Resolution: 8us
  // 11 bits => 3 MSB bits on TC4H, LSB bits on OCR4B
  TC4H = pwm >> 8;
  OCR4A = pwm & 0xFF; // 2.0 or 2.3  boards servo2 output
  OCR4D = pwm & 0xFF; // 2.1 or 2.4  boards servo2 output
}

// output : Battery voltage*10 (aprox) and noise filtered
int BROBOT_readBattery(bool first_time)
{
  BATT_VOLT_ACTUALLY = analogRead(5) / BATT_VOLT_FACTOR;
  //BATT_VOLT_ACTUALLY_PER = (BATT_VOLT_ACTUALLY - BATT_MIN_VOLT) / (BATT_MAX_VOLT - BATT_MIN_VOLT);
  if (first_time)
  {
    //BATT_VOLT_FIRST = BATT_VOLT_ACTUALLY;
    battery = BATT_VOLT_ACTUALLY * 10;
  }
  else
    battery = (battery * 9 + BATT_VOLT_ACTUALLY * 10) / 10;
  return battery;
}
