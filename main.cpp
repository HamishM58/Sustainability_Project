#include <Arduino.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>

//Conveyor
#define ConveyorDirPin 1
#define ConveyorStpPin 2
#define ConveyorEnbPin 3
#define ConveyorSpeed 200
#define ConveyorAcceleration 100
#define ConveyorDiametermm 200

AccelStepper Conveyor(AccelStepper::DRIVER,ConveyorStpPin,ConveyorDirPin);

//Paddles
#define OpenPosition 125
#define ServoMin 1000
#define ServoMax 2000
#define PaddlePWMPin1 1
#define PaddlePWMPin2 2
#define PaddlePWMPin3 3
#define PaddlePWMPin4 4
#define PaddlePWMPin5 5

Servo Paddle1;
Servo Paddle2;
Servo Paddle3;
Servo Paddle4;
Servo Paddle5;

void setup() {
  Conveyor.setEnablePin(ConveyorEnbPin);
  Conveyor.setAcceleration(ConveyorAcceleration);
  Conveyor.setMaxSpeed(ConveyorSpeed);

  Paddle1.setPeriodHertz(50);
  Paddle2.setPeriodHertz(50);
  Paddle3.setPeriodHertz(50);
  Paddle4.setPeriodHertz(50);
  Paddle5.setPeriodHertz(50);

  Paddle1.attach(PaddlePWMPin1, ServoMin, ServoMax);
  Paddle2.attach(PaddlePWMPin2, ServoMin, ServoMax);
  Paddle3.attach(PaddlePWMPin3, ServoMin, ServoMax);
  Paddle4.attach(PaddlePWMPin4, ServoMin, ServoMax);
  Paddle5.attach(PaddlePWMPin5, ServoMin, ServoMax);
}

void loop() {
  // put your main code here, to run repeatedly:
  Conveyor.run();
}

void MoveConveyor(int mm, bool dir){
  if(dir){
    Conveyor.move((PI*ConveyorDiametermm/mm)*360);
  }
  else{
    Conveyor.move(-(PI*ConveyorDiametermm/mm)*360);
  }
} 

void OpenPaddle(int paddle){
  switch(paddle){
    case 1:
      Paddle1.write(OpenPosition);
      break;
    case 2:
      Paddle2.write(OpenPosition);
      break;
    case 3:
      Paddle3.write(OpenPosition);
      break;
    case 4:
      Paddle4.write(OpenPosition);
      break;
    case 5:
      Paddle5.write(OpenPosition);
      break;
  }
}

void ClosePaddle(int paddle){
  switch(paddle){
    case 1:
      Paddle1.write(0);
      break;
    case 2:
      Paddle2.write(0);
      break;
    case 3:
      Paddle3.write(0);
      break;
    case 4:
      Paddle4.write(0);
      break;
    case 5:
      Paddle5.write(0);
      break;
  }
}
