#include <AFMotor.h>
#include <NewPing.h>// ultrasonic sensor library

#define SONAR_NUM 4      // Number of sensors.
#define MAX_DISTANCE 400 // Maximum distance (in cm) to ping.

//Motors initialization
AF_DCMotor frtmotor(4, MOTOR34_64KHZ);
AF_DCMotor fltmotor(3, MOTOR34_64KHZ);
AF_DCMotor rightmotor(1, MOTOR12_64KHZ);
AF_DCMotor leftmotor(2, MOTOR12_64KHZ);

//4 ultrasonic sensors
const byte ftrig = 14;
const byte fecho = 14;
const byte rtrig = 18;
const byte recho = 18;
const byte btrig = 16;
const byte becho = 16;
const byte ltrig = 19;
const byte lecho = 19;


//taking time into consideration
unsigned int currentTime;
const unsigned int alertTime;
byte vel;
float velocity;

//speeds constants
const byte llfd = 20;
const byte sd = 15;

//sides and declaration
byte fd, rd, bd, ld;
unsigned int sides[] = {fd, rd, bd, ld};
String range[] = {"frontdist", "rightdist", "backdist", "leftdist"};

//led pins
const byte rled = 8;
const byte yled = 15;
//const byte sensorNum = 4;



//ultrasonic sensor iniatialization, moving clockwise.
NewPing sonar[SONAR_NUM] = {
  NewPing(ftrig, fecho, MAX_DISTANCE),//front sensor
  NewPing(rtrig, recho, MAX_DISTANCE),//right sensor
  NewPing(btrig, becho, MAX_DISTANCE),//back sensor
  NewPing(ltrig, lecho, MAX_DISTANCE) //left sensor
};

//functions definition
void pinger(){
  delay(60);
   for (uint8_t i = 0; i < SONAR_NUM; i++) { //moving through the array
    
    Serial.print(range[i]);
    Serial.print(" = ");
     sides[i] = (sonar[i].ping_cm());
    Serial.print(sides[i]);
    Serial.print("cm ");
    Serial.print("\t\t");
    }
  Serial.println("");
}
void japaa(char what, byte vel)
{ 
  frtmotor.setSpeed(vel);
  fltmotor.setSpeed(vel);
  leftmotor.setSpeed(vel);
  rightmotor.setSpeed(vel);
  leftmotor.run(what) ;
  rightmotor.run(what);
  frtmotor.run(what);
  fltmotor.run(what);
}

//code for turning left incase of obstruction
void turnleft(char what, byte vel)
{
    onyled();
      rightmotor.setSpeed(vel);
      frtmotor.setSpeed(vel);
      leftmotor.setSpeed(vel);
      fltmotor.setSpeed(vel);
      rightmotor.run(what);
      frtmotor.run(what);
      leftmotor.run(RELEASE);
      fltmotor.run(RELEASE);
     Serial.println("Turning Left\t"); Serial.print(vel);
    
  }

//code for turning right...
void turnright(char what, byte vel)
{
    onyled();
      leftmotor.setSpeed(vel);
      fltmotor.setSpeed(vel);
      rightmotor.setSpeed(vel);
      frtmotor.setSpeed(vel);
      leftmotor.run(what);
      fltmotor.run(what);
      rightmotor.run(RELEASE);
      frtmotor.run(RELEASE);
     Serial.println("Turning Right\t"); Serial.print(vel);
  }  


void reverse(){
    //float velocity;
    velocity = 255*(1-exp(-0.029*sides[2]));
    byte vel = round(velocity);
    while(sides[1]<sd && sides[3]<sd){
      japaa(BACKWARD, vel);
      delay (50);
      Serial.println("Reversing..."); Serial.print(vel);
    } 
   chksides();
}

void onrled()
{
  digitalWrite(rled, HIGH);
}
void onyled()
{
  digitalWrite(yled, HIGH);
}
void offrled()
{
  digitalWrite(rled, LOW);
}
void offyled()
{
  digitalWrite(yled, LOW);
}
//blinking led upon alert of turning or stopping..
void blinkled(int led)
{
   pinMode(led,OUTPUT);
  digitalWrite(led,HIGH);
  delay(100);
  digitalWrite(led, LOW);
}


//code for checking sides in a loop
void chksides()
{
  if ((sides[3] > sides[1]) && (sides[1]>llfd)) // if left distance is greater than right distance
  {
  
    onyled(); //turn on yellow led to indicate change in direction
    turnleft(FORWARD,240);
     }
  else if ((sides[3] < sides[1]) && (sides[1]>llfd)) //if right distance is greater than left distance, turn right
  {
    
    onyled();// turn on yellow led to indicate change in direction
     turnright(FORWARD, 240);
      }
 else if ((sides[1]<=sd) && (sides[3]<=sd)){
    reverse();
  }
  }

void chkdir() {
  pinger();
  if ((sides[1]<=10) && (sides[3]>10)){
    velocity = 255*(exp(-0.0019*sides[1]));
    vel = round(velocity);
    turnleft(FORWARD, vel);
  }
  else if ((sides[3]<=10) && (sides[1]>10)){
    velocity = 255*(exp(-0.0019*sides[3]));
    vel = round(velocity);
    turnright(FORWARD, vel);
  }
  else if (sides[0] <= llfd)
  {
    //check if either you go left or right;
     chksides();  
  }
  else if (sides[0]>llfd){
    //float velocity;
    velocity = 255*(1- exp(-0.029*sides[0]));
     vel = round(velocity);
    japaa(FORWARD, vel);
     Serial.println(" FORWARD, \t"); Serial.print(vel);
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);  //to initialize all sensors
}
void loop() {
  chkdir();
}
