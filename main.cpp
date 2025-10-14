#include <Arduino.h>
#include <AccelStepper.h>

//WiFI
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);
void handleRoot() {
  server.send(200, "text/plain", "Ready");
}
void handleGet() {
  if (server.hasArg("data")) {
    String data = server.arg("data");
    Serial.println(data);
    Sort(data.toInt());
  }
  server.send(200, "text/plain", "Data Received");
}
void handlePost() {
  server.send(200, "text/plain", "Processing Data");
}
void handleUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.println("Receiving data:");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Serial.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    server.send(200, "text/plain", "Data: ");
  }
}



//Conveyor
#define ConveyorDirPin 13
#define ConveyorStpPin 12

#define ConveyorSpeed 300
#define ConveyorAcceleration 2000
#define ConveyorDiametermm 30.5

//Wiper
#define WiperDirPin 11
#define WiperStpPin 10

#define WiperBounds 600

#define WiperSpeed 1500
#define WiperAcceleration 2000



AccelStepper Conveyor(AccelStepper::DRIVER,ConveyorStpPin,ConveyorDirPin);
AccelStepper Wiper(AccelStepper::DRIVER,WiperStpPin,WiperDirPin);


void setup() {
  Serial.begin(115200);

  Conveyor.setAcceleration(ConveyorAcceleration);
  Conveyor.setMaxSpeed(ConveyorSpeed);

  Wiper.setAcceleration(WiperAcceleration);
  Wiper.setMaxSpeed(WiperSpeed);

  //Wifi
  WiFi.softAP("ESP32");
  server.on("/", handleRoot);
  server.on("/get", HTTP_GET, handleGet);
  server.on("/post", HTTP_POST, handlePost, handleUpload);
  server.begin();
}

void loop() {
  server.handleClient();
  if(Serial.available()){
    int number = Serial.parseInt();
    if(number>0){
      Serial.println(String(number));
      Sort(number);
    }
  }
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

void Sort(int bucket){
  switch(bucket){
    case 1:
      MoveConveyor(50,1);
      Wipe(1);
      break;
    case 2:
      MoveConveyor(100,1);
      Wipe(1);
      break;
    case 3:
      MoveConveyor(150,1);
      Wipe(1);
      break;
    case 4:
      MoveConveyor(200,1);
      Wipe(1);
      break;
    case 5:
      MoveConveyor(250,1);
      Wipe(1);
      break;
    case 6:
      MoveConveyor(50,1);
      Wipe(0);
      break;
    case 7:
      MoveConveyor(100,1);
      Wipe(0);
      break;
    case 8:
      MoveConveyor(150,1);
      Wipe(0);
      break;
    case 9:
      MoveConveyor(200,1);
      Wipe(0);
      break;
    default:
      MoveConveyor(250,1);
      Wipe(0);
      break;
  }
}
