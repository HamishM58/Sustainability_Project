#include <Arduino.h>
#include <AccelStepper.h>
//Conveyor
#define ConveyorDirPin 10
#define ConveyorStpPin 11

#define ConveyorSpeed 300
#define ConveyorAcceleration 2000
#define ConveyorDiametermm 30.5

//Wiper
#define WiperDirPin 3
#define WiperStpPin 46

#define WiperBounds 600

#define WiperSpeed 1500
#define WiperAcceleration 2000



AccelStepper Conveyor(AccelStepper::DRIVER,ConveyorStpPin,ConveyorDirPin);
AccelStepper Wiper(AccelStepper::DRIVER,WiperStpPin,WiperDirPin);

void setup() {
  Conveyor.setAcceleration(ConveyorAcceleration);
  Conveyor.setMaxSpeed(ConveyorSpeed);

  Wiper.setAcceleration(WiperAcceleration);
  Wiper.setMaxSpeed(WiperSpeed);

}

void loop() {
  MoveConveyor(150,1);
  Wipe(true);
  MoveConveyor(200,1);
  Wipe(true);

  while(true){}
}

void Wipe(bool dir){
  if(dir){
    Wiper.move(WiperBounds);
    Serial.println("Wipe Left");
  }
  else{
    Wiper.move(-(WiperBounds));
    Serial.println("Wipe Right");
  }
  while(Wiper.isRunning()){
    Wiper.run();
  }
  delay(100);
  Wiper.moveTo(0);
  while(Wiper.isRunning()){
    Wiper.run();
  }
}

void MoveConveyor(float mm, bool dir){
  if(dir){
    Conveyor.move((mm/(PI*ConveyorDiametermm))*200);
  }
  else{
    Conveyor.move(-(mm/(PI*ConveyorDiametermm))*200);
  }
  while(Conveyor.isRunning()){
    Conveyor.run();
  }
} 

