/******************************************
 * Name:SF-SR02 Ultrasonic Module
 * Function:Detection the distance
For any technical questions, visit http://www.sunfounder.com/forum
 **********************************************/
//Email: service@sunfounder.com
//Website: www.sunfounder.com

#include <Wire.h>

#define SIG A0 //SIG attach to A0 of control board
unsigned long rxTime;
float distance;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
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
  Serial.print("distance: "); //print distance: 
  Serial.print(distance); //print the distance
  Serial.println("CM"); //and the unit
  delay(10);
}
