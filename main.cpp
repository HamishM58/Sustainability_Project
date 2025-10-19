#include <Arduino.h>
#include <AccelStepper.h>

#include "AS726X.h"
#include "Wire.h"

//WiFI
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);
void handleRoot() {
  server.send(200, "text/plain", "Ready");
  IdentifyPlastic();
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


AS726X sensor;
int bbyte = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Wire.begin();
  while(Scan()==false);
  Serial.println("I2C connect!");
  delay(1000);
  while(sensor.begin()==false);
  Serial.println("AS726X Sensor connect!");
  delay(1000);

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
  if (Serial.available() > 0) {
    bbyte = Serial.read();
    IdentifyPlastic();
  }
}

void TakeReading(){
  delay(1000);
  sensor.enableBulb();
  delay(1000);
  sensor.takeMeasurements();
  delay(1000);
  sensor.disableBulb();
}

void PrintReading(){
  Serial.print("R:");
  Serial.println(sensor.getCalibratedR(), 2);
  Serial.print("S:");
  Serial.println(sensor.getCalibratedS(), 2);
  Serial.print("T:");
  Serial.println(sensor.getCalibratedT(), 2);
  Serial.print("U:");
  Serial.println(sensor.getCalibratedU(), 2);
  Serial.print("V:");
  Serial.println(sensor.getCalibratedV(), 2);
  Serial.print("W:");
  Serial.println(sensor.getCalibratedW(), 2);
  Serial.println();
}

void IdentifyPlastic(){
  TakeReading();
  delay(100);
  float R = sensor.getCalibratedR();
  float S = sensor.getCalibratedS();
  float T = sensor.getCalibratedT();
  float U = sensor.getCalibratedU();
  float V = sensor.getCalibratedV();
  float W = sensor.getCalibratedW();

  float RS = R/S;
  float ST = S/T;
  float TU = T/U;
  float UV = U/V;
  float VW = V/W;

  float Vector[5] = {RS,ST,TU,UV,VW};
  //Serial.println(String(Vector[0])+" "+String(Vector[1])+" "+String(Vector[2])+" "+String(Vector[3])+" "+String(Vector[4]));

  float VectorBelt[5] = {5.035,0.9175,3.4725,2.0175,2.31};
  float VectorHDPE[5] = {4.435,1.7025,3.13,1.34,1.6375};
  float VectorPP[5] = {18.442,0.598,6.2,1.072,2.586};
  float VectorPLA[5] = {2.964,2.396,2.39,1.364,1.498};
  float VectorPETG[5] = {2.648,2.25,2.624,1.516,1.506};
  float VectorABS[5] = {3.912,2.002,2.664,1.298,1.732};

  float VectorPETGB[5] = {11.712,0.434,2.37,1.926,3.44};
  float VectorPLAR[5] = {2.916,2.216,2.564,1.506,1.5};
  //float VectorERROR[5] = {0,0,0,0,0};

  //float VectorPETE[5] = {5.45,0.9775,4.57,1.44,1.925};
  
  
  float BELT = Edistance(Vector, VectorBelt);
  float HDPE = Edistance(Vector, VectorHDPE);
  float PP = Edistance(Vector, VectorPP);
  float PLA = Edistance(Vector, VectorPLA);
  float PETG = Edistance(Vector, VectorPETG);
  float ABS = Edistance(Vector, VectorABS);

  float PLAR = Edistance(Vector, VectorPLAR);
  float PETGB = Edistance(Vector, VectorPETGB);

  //
  float minimum = min(min(min(min(BELT, HDPE),min(PP, PLA)),min(PETG,ABS)),min(PLAR,PETGB));
  if(minimum == BELT){
    Serial.println("BELT");
  }
  else if(minimum == HDPE){
    Serial.println("HDPE");
    Sort(1);
  }
  else if(minimum == PP){
    Serial.println("PP");
    Sort(2);
  }
  else if(minimum == PLA){
    Serial.println("PLA");
    Sort(3);
  }
  else if(minimum == PETG){
    Serial.println("PETG");
    Sort(4);
  }
  else if(minimum == ABS){
    Serial.println("ABS");
    Sort(5);
  }
  else if(minimum == PETGB){
    Serial.println("PETGB");
    Sort(4);
  }
  else if(minimum == PLAR){
    Serial.println("PLAR");
    Sort(3);
  }
  else{
    Serial.println("Error, Not determined. Catch Bin");
    Sort(0);
  }


  /*
  Serial.print("BELT: ");
  Serial.println(String(BELT));
  Serial.print("HDPE: ");
  Serial.println(String(HDPE));
  Serial.print("PP: ");
  Serial.println(String(PP));
  Serial.print("PLA: ");
  Serial.println(String(PLA));
  Serial.print("PA: ");
  Serial.println(String(PA));
  */
}
float Edistance(float* A, float* B) {
    float sum = 0;
    for (int i = 0; i < 5; i++) {
        float diff = A[i] - B[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}


bool Scan(){
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
      Serial.print("0");
      Serial.println(address,HEX);
      return true;
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0){
    Serial.println("No I2C devices found\n");
    return false;
  }
  else{
    Serial.println("done\n");
  }
  delay(1000);
}
void Wipe(bool dir){
  if(dir){
    Wiper.move(WiperBounds);
  }
  else{
    Wiper.move(-(WiperBounds));
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
