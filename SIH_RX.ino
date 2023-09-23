#include <SPI.h>
#include <LoRa.h>
#include "WiFi.h"
#include <HTTPClient.h>

const char* ssid = "REALME";         // change SSID
const char* password = "12345678";    // change password

String GOOGLE_SCRIPT_ID = "AKfycbx2xiza6vZXYz553QxGeafJQsWrCFVsEpmBv7HQawsb0vyeaZ2FXiJLWslAdwUCTyMt";

#define ss 5
#define rst 14
#define dio0 2

#define BAND 433E6

String sensorValue1;
String sensorValue2;


void getLoRaData() {
  Serial.print("Lora packet received: ");
  
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    Serial.print(LoRaData); 
    
    int pos1 = LoRaData.indexOf('/');

     sensorValue1 = LoRaData.substring(0, pos1);
     sensorValue2 = LoRaData.substring(pos1 +1, LoRaData.length());
  }
  int rssi = LoRa.packetRssi();
  Serial.print(" with RSSI ");    
  Serial.println(rssi);
}

void printdata()
{
  Serial.print("AirQua1=");
  Serial.print(sensorValue1);               // prints the value read
  Serial.println(" PPM");
  Serial.print("AirQua2=");
  Serial.print(sensorValue2);               // prints the value read
  Serial.println(" PPM");
}

void senddata()
{
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+ "&SensorVal1=" + String(sensorValue1) +"&SensorVal2=" + String(sensorValue2) ;
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }
    //---------------------------------------------------------------------
    http.end();
  
}

void setup()
{
  Serial.begin(9600);

  int counter;
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");

    Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
}

void loop(){
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    getLoRaData();
    printdata();
    senddata();
  }
}
