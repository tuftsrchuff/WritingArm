#include <AccelStepper.h>
#include <MultiStepper.h>
#include <ESP32Servo.h>

#define SERVO_PIN 13

int servoInitPos = 0;
int servoFinalPos = 60;
// Connections to A4988
const int dirPinLeft = 26;  // Direction
const int stepPinLeft = 27; // Step

const int dirPinRight = 33;
const int stepPinRight = 32;

const int buttonPin = 14;

double short_arm = 10;
double long_arm = 15;
double loc_left = 0;
double loc_right = 10;

// Motor steps per rotation
const int STEPS_PER_REV = 200;

bool ran = false;
int buttonLastState;
AccelStepper stepperLeft(AccelStepper::DRIVER, stepPinLeft, dirPinLeft);
AccelStepper stepperRight(AccelStepper::DRIVER, stepPinRight, dirPinRight);
Servo servoMotor;

MultiStepper steppers;
 
void setup() {
  delay(1000);
  Serial.begin(115200);

  stepperLeft.setMaxSpeed(50);   // this limits the value of setSpeed(). Raise it if you like.
  stepperLeft.setSpeed(20);    // runSpeed() will run the motor at this speed - set it to whatever you like.
  stepperLeft.setAcceleration(30);

  stepperRight.setMaxSpeed(50);   // this limits the value of setSpeed(). Raise it if you like.
  stepperRight.setSpeed(20);     // runSpeed() will run the motor at this speed - set it to whatever you like.
  stepperRight.setAcceleration(30);
 
  // Setup the pins as Outputs
  pinMode(stepPinLeft,OUTPUT);
  pinMode(dirPinLeft,OUTPUT);
  pinMode(stepPinRight, OUTPUT);
  pinMode(dirPinRight, OUTPUT);
  Serial.println("Accel stepper script");

  pinMode(buttonPin, INPUT_PULLUP); // Setting up button
  buttonLastState = HIGH;
  steppers.addStepper(stepperLeft);
  steppers.addStepper(stepperRight);
  servoMotor.attach(SERVO_PIN);  // Setting up servo
  // Initialize servo position
  servoMotor.write(servoInitPos);
  delay(1000);

}
void loop() {
    servoMotor.write(servoInitPos);
    delay(1000);

  int buttonCurState = digitalRead(buttonPin);

  if(buttonLastState == LOW && buttonCurState == HIGH){
    servoMotor.write(servoInitPos);
    delay(1000);
    // stepperRight.runToNewPosition(300);
    // stepperRight.runToNewPosition(0);
  long positions[2]; // Array of desired stepper positions

  Serial.println("Button hit!");
  
  positions[0] = -10;
  positions[1] = 10;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition(); // Blocks until all are in position
  delay(1000);
  
  // Move to a different coordinate
  positions[0] = 10;
  positions[1] = -10;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition(); // Blocks until all are in position
  delay(1000);


  // Move to a different coordinate
  positions[0] = 0;
  positions[1] = 0;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition(); // Blocks until all are in position
  delay(1000);
  servoMotor.write(servoFinalPos);
  delay(1000);
}
  buttonLastState = buttonCurState;
}

void move_steppers(int step_left, int step_right){
  int leftInc = 0;
  int rightInc = 0;
    if(stepperLeft.currentPosition() < step_left){
      leftInc = -1;
    } else if(stepperLeft.currentPosition() > step_left){
      leftInc = 1;
    }

    if(stepperRight.currentPosition() < step_right){
      rightInc = 1;
    } else if(stepperRight.currentPosition() > step_right){
      rightInc = -1;
    }

    while(stepperRight.currentPosition() != step_right && stepperLeft.currentPosition() != step_left){
      Serial.print("Left");
        Serial.println(stepperLeft.currentPosition());
        Serial.print("Right");
        Serial.println(stepperRight.currentPosition());
      stepperLeft.runToNewPosition(stepperLeft.currentPosition() + leftInc);
      stepperRight.runToNewPosition(stepperRight.currentPosition() + rightInc);

    }


    Serial.println("Break while");
    Serial.println("Left position");
    Serial.println(stepperLeft.currentPosition());
    Serial.println("To get to");
    Serial.println(step_left);
    stepperLeft.runToNewPosition(step_left);

    Serial.println("Right position");
    Serial.println(stepperRight.currentPosition());
    Serial.println("To get to");
    Serial.println(step_right);
    stepperRight.runToNewPosition(step_right);
}



int angle_to_step(double angle){
  int steps = angle / 1.8;
  return steps;
}

double inverse_kin_angle_left(double x, double y){
  //Inner Angle forming triangle to tip from base
  double dist = sqrt(sq(x - loc_left) + sq(y));
  double alpha = acos(x/dist);
 
  double top = sq(long_arm) - sq(short_arm) - sq(dist);
  double bottom = -2*short_arm*dist;
  double beta = acos(top/bottom);
  double rad_to_deg = (alpha+beta) * 180/M_PI;
  Serial.println("Original value left");
  Serial.println(rad_to_deg);

  return rad_to_deg - 90;
}

double inverse_kin_angle_right(double x, double y){
  //Inner Angle forming triangle to tip from base
  double dist = sqrt(sq(x - loc_right) + sq(y));
  double alpha = acos((-x + loc_right)/dist);
 
  double top = sq(long_arm) - sq(short_arm) - sq(dist);
  double bottom = -2*short_arm*dist;
  double beta = acos(top/bottom);
  double rad_to_deg = (alpha+beta) * 180/M_PI;
  Serial.println("Original value right");
  Serial.println(rad_to_deg);
  return 90 - rad_to_deg;
}

double steps_to_angle(int steps){
  return steps * 1.8;
}