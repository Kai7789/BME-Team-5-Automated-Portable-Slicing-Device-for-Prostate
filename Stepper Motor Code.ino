/*
The circuit:
    - LED attached from pin 11 (Z+ on CNC board) to ground through 220 ohm resistor
    - pushbutton attached to pin 10 (Y+ on CNC board) from +5V; 10K resistor attached to pin 10 from ground
*/

#include <AccelStepper.h>

const int enablePin = 8;
const int dirX = 5;
const int stepX = 2;
const int dirY = 6;
const int stepY = 3;
const int dirZ = 7;
const int stepZ = 4;

AccelStepper stepper1(1, stepY, dirY, 10); // stepper1 ~ X ~ back and forth
AccelStepper stepper2(1, stepZ, dirZ);     // stepper2 ~ Z ~ up and down

const int buttonPin = 10; // Z+ on CNC board
const int ledPin = 11;    // Y+ on CNC board
int buttonState = 0;

const int liftHeight = 1500; // 1000 steps = 4 cm, 1500 steps = 6 cm
const int liftSpeed = 500;

bool stepper1Stopped = true;
bool stepper2Stopped = true;

int i = 1;
int repetition = 36;

const int speed2 = -300; // speed of up and down motor
int speed1 = 50 * repetition * speed2 / liftHeight;       
// 50*speed2*repetition/liftHeight. speed of back and forth motor. -50*speed2/liftHeight for single trip

void setup()
{
    // pins for motors
    pinMode(stepX, OUTPUT);
    pinMode(dirX, OUTPUT);
    pinMode(stepZ, OUTPUT);
    pinMode(dirZ, OUTPUT);
    pinMode(enablePin, OUTPUT);
    digitalWrite(enablePin, LOW); // Set enable pin to LOW to start operation

    // pins for the button, light, and speaker
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT);

    stepper1.setAcceleration(5000);
    stepper2.setAcceleration(5000);
}

void loop()
{
    if (stepper1Stopped == true && stepper2Stopped == true)
    {
        while (i < (repetition + 1))
        {
            // Stepper1 goes back and forth
            stepper1.moveTo(50);
            stepper1.setMaxSpeed(speed1);
            stepper2.move(-liftHeight / repetition);
            stepper2.setMaxSpeed(speed2);

            digitalWrite(ledPin, HIGH);
            wait_for_stepper_completion();

            stepper1.moveTo(0);
            stepper1.setMaxSpeed(speed1);
            stepper2.move(-liftHeight / repetition);
            stepper2.setMaxSpeed(speed2);
            wait_for_stepper_completion();

            i = i + 2;
        }

        // back to initial position
        stepper2.moveTo(0);
        stepper2.setMaxSpeed(liftSpeed);
        wait_for_stepper_completion();
        digitalWrite(ledPin, LOW);
        
        stepper1Stopped = false;
        stepper2Stopped = false;
    }
}

void wait_for_stepper_completion()
{
    while (stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0)
    {
        buttonState = digitalRead(buttonPin);

        if (buttonState == HIGH)
        {
            digitalWrite(ledPin, LOW);
            stepper1.setMaxSpeed(0);
            stepper2.setMaxSpeed(0);
        }

        stepper1.run();
        stepper2.run();
    }
}
