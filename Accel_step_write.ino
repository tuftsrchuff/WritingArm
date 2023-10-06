#include <AccelStepper.h>
#include <MultiStepper.h>
#include <ESP32Servo.h>
#include <array>
#include <string>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
using namespace std;

//Setting pins for board
#define SERVO_PIN 26 
int servoPenDown = 0; //Fully horizontal pos
int servoPenUp = 60; //Upright position, lifts pen
const int dirPinLeft = 26;  // Direction
const int stepPinLeft = 27; // Step

const int dirPinRight = 33;
const int stepPinRight = 32;

const int buttonPin = 14;

//Arm lengths and coordinates
double short_arm = 10;
double long_arm = 15;
double loc_left = 0;
double loc_right = 10;

// Motor steps per rotation, using full steps
const int STEPS_PER_REV = 200;

// Change the ssid and password to something else 
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789"; 

//Whether or not sad face provided from python
bool sadFaceR = false;
bool sadFaceT = false;
int person = 1;

AsyncWebServer server(80);

bool ran = false;
int buttonLastState;
AccelStepper stepperLeft(AccelStepper::DRIVER, stepPinLeft, dirPinLeft);
AccelStepper stepperRight(AccelStepper::DRIVER, stepPinRight, dirPinRight);
Servo servoMotor;

MultiStepper steppers;

//Letter mappings
double letterR[11][2] = {
{5, 18},
{5, 8},
{5, 18},
{10, 18},
{10, 15},
{5, 15},
{6, 14},
{7,13},
{8, 12},
{9, 11},
{10, 8}
};

double letterH[11][2] = {
{5, 18},
{5, 8},
{5, 18},
{10, 18},
{10, 15},
{5, 15},
{6, 14},
{7,13},
{8, 12},
{9, 11},
{10, 8}
};

double letterT[11][2] = {
{5, 18},
{5, 8},
{5, 18},
{10, 18},
{10, 15},
{5, 15},
{6, 14},
{7,13},
{8, 12},
{9, 11},
{10, 8}
};

double letterB[11][2] = {
{5, 18},
{5, 8},
{5, 18},
{10, 18},
{10, 15},
{5, 15},
{6, 14},
{7,13},
{8, 12},
{9, 11},
{10, 8}
};

double sadFaceLoc[11][2] = {
{5, 18},
{5, 8},
{5, 18},
{10, 18},
{10, 15},
{5, 15},
{6, 14},
{7,13},
{8, 12},
{9, 11},
{10, 8}
};

double happyFaceLoc[11][2] = {
{5, 18},
{5, 8},
{5, 18},
{10, 18},
{10, 15},
{5, 15},
{6, 14},
{7,13},
{8, 12},
{9, 11},
{10, 8}
};

void setup() {
  delay(1000);
  Serial.begin(115200); // basic setup for serta communication when ESp32 is connected to pc via USB
  // Setting up the ESP32 Hosted wifi
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: "); Serial.println(IP); // displays the IP address for the ESP32, by default it is 192.168.4.1

   // creates server pages for sadface and happyface
  server.on("/sadface",HTTP_POST,[](AsyncWebServerRequest * request){},
    NULL,[](AsyncWebServerRequest * request, uint8_t *data_in, size_t len, size_t index, size_t total) {
      Serial.print("Sad face hit");
      request->send_P(200, "text/html","OK");
      //Triggers Ryans initials/emotions if first request, Talha's second
      if(person == 1){
        sadFaceR == true;
        person++;
        runWriteRH();
      } else {
        sadFaceT == true;
        person++;
        runWriteTB();
      }
      
  });

  //Triggers Ryans initials/emotions if first request, Talha's second
  server.on("/happyface",HTTP_POST,[](AsyncWebServerRequest * request){},
    NULL,[](AsyncWebServerRequest * request, uint8_t *data_in, size_t len, size_t index, size_t total) {
      Serial.print("Happy face hit");
      request->send_P(200, "text/html","OK");
      if(person == 1){
        sadFaceR == false;
        person++;
        runWriteRH();
      } else {
        sadFaceT == false;
        runWriteTB();
      }
  });

  // Start server
  server.begin();


   // runSpeed() will run the motor at this speed - set it to whatever you like.
  stepperLeft.setMaxSpeed(100);
  stepperLeft.setSpeed(15); 
  stepperLeft.setAcceleration(20);

  stepperRight.setMaxSpeed(100);   // this limits the value of setSpeed(). Raise it if you like.
  stepperRight.setSpeed(15);
  stepperRight.setAcceleration(20);
 
  // Setup the pins as Outputs
  pinMode(stepPinLeft,OUTPUT);
  pinMode(dirPinLeft,OUTPUT);
  pinMode(stepPinRight, OUTPUT);
  pinMode(dirPinRight, OUTPUT);
  Serial.println("Accel stepper script");

  pinMode(buttonPin, INPUT_PULLUP); // Setting up button
  buttonLastState = HIGH;

  //Setting up multistepper to run steppers simultaneously
  steppers.addStepper(stepperLeft);
  steppers.addStepper(stepperRight);
  servoMotor.attach(SERVO_PIN);  // Setting up servo
  // Initialize servo position
  servoMotor.write(servoPenUp);
  delay(1000);

  //Initialize steppers to initial position
  stepperLeft.runToNewPosition(0);
  stepperRight.runToNewPosition(0);

}
void loop() {
      
}

//Writing for Ryan's initials and emotions
void runWriteRH(){
  //Trigger all
  servoMotor.write(servoPenUp);
  drawR();
  drawH();
  if(sadFaceR == true){
    drawSmiley(1);
  } else {
    drawFrowny(1);
  }
  servoMotor.write(servoPenUp);

}

//Writing for Talha's initials and emotions
void runWriteTB(){
  servoMotor.write(servoPenUp);
  drawT();
  drawB();
  if(sadFaceT == true){
    drawSmiley(2);
  } else {
    drawFrowny(2);
  }
  servoMotor.write(servoPenUp);
}

//Loop over letterR point mappings and write letter
void drawR(){
  int numItems = sizeof(letterR)/ (2 * sizeof(double));

  //Move to respective positions
  for(int i = 0; i < numItems; i++){
    double x = letterR[i][0];
    double y = letterR[i][1];
    moveToPos(x, y, true, -5, 0);
    delay(500);
  }

  servoMotor.write(servoPenUp);
}

//Loop over letterH point mappings and write letter
void drawH(){
  int numItems = sizeof(letterH)/ (2 * sizeof(double));

  for(int i = 0; i < numItems; i++){
    double x = letterH[i][0];
    double y = letterH[i][1];
    moveToPos(x, y, true, 0, 0);
    delay(500);
  }

  servoMotor.write(servoPenUp);
}

//Loop over letterT point mappings and write letter
void drawT(){
  int numItems = sizeof(letterT)/ (2 * sizeof(double));

  for(int i = 0; i < numItems; i++){
    double x = letterT[i][0];
    double y = letterT[i][1];
    moveToPos(x, y, true, 5, 0);
    delay(500);
  }

  servoMotor.write(servoPenUp);
}

//Loop over letterB point mappings and write letter
void drawB(){
  int numItems = sizeof(letterT)/ (2 * sizeof(double));

  for(int i = 0; i < numItems; i++){
    double x = letterB[i][0];
    double y = letterB[i][1];
    moveToPos(x, y, true, 10, 0);
    delay(500);
  }

  servoMotor.write(servoPenUp);
}

//Draw smiley face for respective person
void drawSmiley(int personNum){

  //Emotion needs to be translated to under person's name
  int transX = 0;
  int transY = 0;
  if(personNum == 1){
    transX = -5;
    transY = -8;
  } else {
    transX = 5;
    transY = -8;
  }
  int numItems = sizeof(happyFaceLoc)/ (2 * sizeof(double));

  for(int i = 0; i < numItems; i++){
    double x = happyFaceLoc[i][0];
    double y = happyFaceLoc[i][1];
    moveToPos(x, y, true, transX, transY);
    delay(500);
  }

  servoMotor.write(servoPenUp);
}

//Draw frowny face for respective person
void drawFrowny(int personNum){
  //Emotion needs to be translated to under person's name
  int transX = 0;
  int transY = 0;
  if(personNum == 1){
    transX = -5;
    transY = -8;
  } else {
    transX = 5;
    transY = -8;
  }
  int numItems = sizeof(sadFaceLoc)/ (2 * sizeof(double));

  for(int i = 0; i < numItems; i++){
    double x = sadFaceLoc[i][0];
    double y = sadFaceLoc[i][1];
    moveToPos(x, y, true, transX, transY);
    delay(500);
  }

  servoMotor.write(servoPenUp);
}

//Move to positions used in mappings, update pen movement and translate across X/Y
void moveToPos(double x, double y, bool penDown, double transX, double transY){
  //Translation from initial mapping
  double newX = x + transX;
  double newY = y + transY;
  Serial.print("Going to x pos ");
  Serial.println(newX);
  Serial.print("Going to y pos ");
  Serial.println(newY);
  servoMotor.write(servoPenUp);
  if(penDown == true){
      servoMotor.write(servoPenDown);
      delay(500);
  } else {
      servoMotor.write(servoPenUp);
  }

  //Calculate inverse kinematics for arm angles
  long positions[2];
  double la = inverse_kin_angle_left(x,y);
  double ra = inverse_kin_angle_right(x,y);

  //Go from angles to steps motors need to move
  int steps_left = angle_to_step(la);
  int steps_right = angle_to_step(ra);
  Serial.println(steps_left); //Neg equals CC, pos equals clockwise
  Serial.println(steps_right);


  positions[0] = steps_left;
  positions[1] = steps_right;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition(); // Blocks until all are in position
}

//Translate angle into steps for motor
int angle_to_step(double angle){
  int steps = angle / 1.8;
  return steps;
}

//Calculate inverse kinematic angles for left arm
double inverse_kin_angle_left(double x, double y){
  //Inner Angle forming triangle to tip from base
  double dist = sqrt(sq(x - loc_left) + sq(y));
  double alpha = acos(x/dist);
 
  double top = sq(long_arm) - sq(short_arm) - sq(dist);
  double bottom = -2*short_arm*dist;
  double beta = acos(top/bottom);
  double rad_to_deg = (alpha+beta) * 180/M_PI;

  return rad_to_deg - 90;
}

//Calculate inverse kinematic angles for right arm
double inverse_kin_angle_right(double x, double y){
  //Inner Angle forming triangle to tip from base
  double dist = sqrt(sq(x - loc_right) + sq(y));
  double alpha = acos((-x + loc_right)/dist);
 
  double top = sq(long_arm) - sq(short_arm) - sq(dist);
  double bottom = -2*short_arm*dist;
  double beta = acos(top/bottom);
  double rad_to_deg = (alpha+beta) * 180/M_PI;
  return 90 - rad_to_deg;
}

//Translate steps back to angle
double steps_to_angle(int steps){
  return steps * 1.8;
}