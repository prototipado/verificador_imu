/*
|| @author         Brett Hagman <bhagman@wiring.org.co>
|| @url            http://wiring.org.co/
||
|| @description
|| |
|| | This is an example sketch using the ControlledServo library to control a TinkerKit Braccio
|| | (servo controlled robotic arm).
|| |
|| #
||
|| @license Please see the accompanying LICENSE.txt file for this project.
||
*/

#include "ControlledServo.h"
#include <Servo.h>
#include <arduino.h>

Servo s_shoulder;


const int shoulderPin = 10;

uint8_t angles_1[13] = {60,45,60,90,60,120,0,45,0,90,0,120,0};
uint8_t angles_2[7] = {0,60,120,60,120,0};
uint8_t angles_3[7] = {0,30,60,90,120,0};
uint8_t * angles;
int angleRate = 10; // in msPerDegree
int seq=0;
int speed=0;
ControlledServo shoulder;
bool move = false;

unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

/*
|| Set all servos to a given angle.  All parameters required.
*/
void setAll(uint8_t shoulderAngle)
{
  shoulder.moveTo(shoulderAngle);
}


/*
|| Checks if any joint is moving.
*/
boolean moving()
{
  boolean result;
  result = shoulder.moving();
  return result;
}


/*
|| Updates all servos. Can be used to asynchronously control the servos.
*/
bool update()
{
  bool processing = false;
  processing = shoulder.update();
  return processing;
}

const byte interruptPin = 2;
const byte outPin = 3;
void setup()
{
  unsigned long myTime = millis();
  Serial.begin(9600);
  Serial.println(F("Calibration Cube"));

  s_shoulder.attach(shoulderPin);
  shoulder.setServo(s_shoulder);
  // Set angle change rates
  shoulder.setRate(20);

  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(outPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
  pinMode(LED_BUILTIN, OUTPUT);
  angles=angles_1;

  Serial.print(F("Going to start position..."));
  setAll(0);
  while (moving())
    update();
  setAll(120);
  while (moving())
    update();
  setAll(60);
  while (moving())
    update();  
  Serial.println(F("Done"));
  

  Serial.println(myTime); // prints time since program started
}


void loop()
{
  if(move){
    sequence();
  }
}

void blink() {
  Serial.println("Button pressed...");
 pressedTime = 0;
 while(!digitalRead(interruptPin)){
 pressedTime++;

  Serial.println(pressedTime-0);
  if(((pressedTime-0) > 100)  && (pressedTime-0 < 120)){
      //Serial.println("Change angleRate...");
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);  
    }
   if(((pressedTime-0) > 300)  && ((pressedTime-0) < 350)){
      //Serial.println("Change angle seq...");
      digitalWrite(LED_BUILTIN, LOW);
      delay(400);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(400);
      digitalWrite(LED_BUILTIN, LOW);
      delay(400);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(400);
      digitalWrite(LED_BUILTIN, LOW);   
    }
  }

  
 releasedTime = millis();

 long pressDuration = pressedTime;
 if((pressDuration>100) && (pressDuration<120)){
   //INCREASE SPEED
   speed++;
   switch(speed){
    case 1:
      angleRate=10;
      Serial.print(F("angleRate=5..."));
      break;
    case 2:
      angleRate=10;
      Serial.print(F("angleRate=10..."));
      break;
    case 3:
      angleRate=10;
      Serial.print(F("angleRate=20..."));
      speed=0;
      break;
   }
  }
 if(pressDuration<50){
  //START TEST
  move = true;
  } 
if(pressDuration>350){
   //CHANGE POROFILE
   seq++;
   switch(seq){
    case 1:
      angles=angles_1;
      Serial.print(F("angles_1..."));
      break;
    case 2:
      angles=angles_1;
      Serial.print(F("angles_2..."));
      break;
    case 3:
      angles=angles_1;
      Serial.print(F("angles_3..."));
      seq=0;
      break;
   }
  }
}

void sequence(void){
  //Serial.print(F("Position 1..."));
  digitalWrite(outPin, HIGH);
  shoulder.setRate(angleRate);
  for(int i=0;i<14;i++){
  //  if(i==6)
  //    shoulder.setRate(5);
    setAll(angles[i]);
    while (moving())
      update();
    //Serial.print(F("Position 2..."));
  }
  digitalWrite(outPin, LOW);
  delay(3000);
  Serial.print(F("Going to start position..."));
  shoulder.setRate(20);
  setAll(60);
    while (moving())
      update();
  move = false;
  digitalWrite(outPin, LOW);
}
