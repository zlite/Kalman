/******************************************
 * Test of various filtering methods
 * By Chris Anderson, 2018
 * Based on SF-SR02 Ultrasonic Module sampling code by SunFounder
 */


#include <Wire.h>

#define SIG A0 //SIG attach to A0 of control board
unsigned long rxTime;
float distance;
float revised_distance;
float revised_distance2;
float previous = 0;
float measurement_error = 0.1;
float probability = 0.8;
float olddata = 0;
float olddata2 = 0;
float ema2 = 0; // previous ema
float ema = 0; // current ema


void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
}

float weighted_average(float newdata)  // exponential moving average
/*
EMA [current] = (Value [current] x K) + (EMA [previous] x (1 – K))

Where:
K = 2 ÷(N + 1)
N = the length of the EMA

In our case, we'll set N to 10, so K = 0.1818
 */

{
  float k = 0.1818; // standard exponential weight for ten points
  ema = (newdata * k) + (ema2 * (1-k));
  ema2 = ema; // move current reading to previous reading
  return ema;
}

float average(float newdata) //simple 2-point moving average 
{
  float temp;
  temp = (newdata + olddata2)/2;
  olddata2 = newdata;
  return temp;
}


float kalman(float newdata)  // single-state Kalman
{
  float temp;
  temp = newdata - ((newdata - olddata) * probability);
  olddata = temp;
  return temp;
}

void loop() {
  //set SIG as OUTPUT,start to output trigger signal to the module to start the ranging
  pinMode(SIG, OUTPUT);
  //Genarate a pulse 20uS pulse
  digitalWrite(SIG, HIGH);
  delayMicroseconds(20);
  digitalWrite(SIG, LOW);
  //set SIG as INPUT,start to read value from the module
  pinMode(SIG, INPUT);
  rxTime = pulseIn(SIG, HIGH);//waits for the pin SIG to go HIGH, starts timing, then waits for the pin to go LOW and stops timing
  //  Serial.print("rxTime:");
  //  Serial.println(rxTime);
  distance = (float)rxTime * 34 / 2000.0; //convert the time to distance
  //leave the distance between 2cm-800cm
  if(distance < 2)
  {
    distance=0;
  }
  if(distance > 800)
  {
    distance=0;
  }
//  Serial.print("distance: "); //print distance: 
//  Serial.println(distance); //print the distance
//  Serial.println("CM"); //and the unit

//  Serial.print("Revised distance: ");
  Serial.print(kalman (distance));
  Serial.print(" ");
  Serial.print(average(distance));
  Serial.print(" ");
  Serial.print(weighted_average(distance));
  Serial.print(" ");
  Serial.println(distance);

  delay(20);
}
