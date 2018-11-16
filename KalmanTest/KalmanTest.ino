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

//as a good coding practice it would be better to create a class and keep the following as members of it but it doesn't really make a difference. 
float measurement_error = 0.1;
long now=0;

float probability = 0.8;
float olddata = 0;
float olddata2 = 0;
float ema2 = 0; // previous ema
float ema = 0; // current ema


void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(230400); //waste as little time as possible in printing stuff.
  pinMode(SIG,INPUT); //you might wanna define this pin as input if you're using PulseIn function!
  now = millis();//initialize the 'now' variable
}

/*
So one of the major issues with ultrasonic sensors is that we tend to get spikes in the measured distances,
This can sometimes happen due to the object in question not being wide enough and so the values can jump around significantly.
In order to solve this problem, we need to use a low pass filter. A simple implementation of a low pass filter would be to 
take a weighted average of the new distance and the old distance. The problem with this approach is that this works well only 
in some situations and isn't "adaptive".
So how do we solve this? Well, one way of solving this is to say "the more realistic the change in the reading appears to be, 
the more we can trust it" (which is I guess a one line definition of a kalman filter but whatever). The next question then is,
what do you mean by a realistic change? Well the answer is simple. The change is within some physical bounds. So lets say for
example that the ultrasonic sensor is mounted on a rover (car like thing). The rover can at most move at say 10 m/s (damn fast
for a rover eh?). Another thing to consider is that while the rover can move at 10m/s, the probability that it reaches that 
speed in less than 100ms is quite low (10g's of acceleration). So I guess you could use the estimated acceleration as a means
for determining how much you should trust the reading. Now if I had an accelerometer mounted on that rover I could probably also 
compare the 2 things but that isn't the case so we'll make do with what we have.

The idea is that while the rover can accelerate at 1g, the probability that it actually does is not very high, at least
not as high as the probability that the rover moves at a roughly constant speed with some variations.
*/

#define ACC_BOUND 10 //~1 g of acceleration can be tolerated
float last_v=0;


float melting_icecream(float new_data,float dt)
{
  if(dt == 0)
  {
    dt == 0.1;//prevent Nan
  }
  float v = (new_data - olddata)/dt;
  float acc = (v - last_v)/dt;
  last_v = v;
  //filter stuff begins
  float x1 = 3*(acc - ACC_BOUND)/ACC_BOUND; //scale the difference between physical limit and estimated acceleration
  float x2 = x1/2; //temp variable to save on computation (he says as he raises a variable to it's 10th power in the next line).
  float gain = 0.7/(1 + pow(x1,10)) + 0.3/(1+pow(x2,4));//visualisation (x is the difference between estimated acceleration and ACC_BOUND): 
                            //https://www.wolframalpha.com/input/?i=(0.3%2F(1%2B(x%2F6)%5E10)+)+%2B+(0.7%2F(1%2B+(x%2F3)%5E4))
                            //I pulled this above filter out of my ass. figuratively speaking. the shape of the graph looks 
                            //like melting ice-cream, hence the name
  temp = newdata - (newdata - olddata) * gain;
  olddata = temp;
  return temp;
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
  float dt = (millis() - now)*1e-3;
  now = millis();
  pinMode(SIG, OUTPUT);
  //Genarate a pulse 20uS pulse
  digitalWrite(SIG,LOW);//for a clean high
  digitalWrite(SIG, HIGH);
  delayMicroseconds(20);
  digitalWrite(SIG, LOW);
  //set SIG as INPUT,start to read value from the module
  pinMode(SIG, INPUT);
  rxTime = pulseIn(SIG, HIGH);//waits for the pin SIG to go HIGH, starts timing, then waits for the pin to go LOW and stops timing

  distance = (float)rxTime * 0.0017; //convert the time to distance
  //leave the distance between 2cm-800cm
  if(distance < 2)
  {
    distance=0;
  }
  if(distance > 800)
  {
    distance=0;
  }

  //Serial.print(kalman(distance));
  Serial.print(melting_icecream(distance,dt))
  Serial.print(" ");
  Serial.print(average(distance));
  Serial.print(" ");
  Serial.print(weighted_average(distance));
  Serial.print(" ");
  Serial.println(distance);

  //delay was unnecessary and hence removed.
}
