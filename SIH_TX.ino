#include <SPI.h>
#include <LoRa.h> 

int sensorValue1;
int sensorValue2;

String LoRaMessage = "";

void setup() {
  Serial.begin(9600);
  
  while (!Serial);  
  Serial.println("LoRa Sender");
  if (!LoRa.begin(433E6)) { // or 915E6, the MHz speed of yout module
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {

  sensorValue1 = analogRead(0);       //   read analog input pin 0
  sensorValue2 = analogRead(1);       //   read analog input pin 0
  Serial.print("AirQua1=");
  Serial.print(sensorValue1,   DEC);               // prints the value read
  Serial.println(" PPM");
  Serial.print("AirQua2=");
  Serial.print(sensorValue2,   DEC);               // prints the value read
  Serial.println(" PPM");

  LoRaMessage = String(sensorValue1) + "/" + String(sensorValue2) ;
  LoRa.beginPacket();  
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
  
  Serial.println("LoRa Packet sent with message :");
  Serial.println(LoRaMessage);
  Serial.println("");
  delay(500);
}
