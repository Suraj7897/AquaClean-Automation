#include <SPI.h>
#include <LoRa.h>
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

#include <HTTPClient.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Pranav K"
#define WIFI_PASSWORD "55550000"

//For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyAv6qxIPO4A2JRWFxmv__iblH3iY_5tXTg"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://sih1368-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app


//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

#define ss 5
#define rst 14
#define dio0 2

#define BAND 433E6

String main="";

String sensorValue1;
String sensorValue2;
String formattedDate;
String dayStamp;
String timeStamp;

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

void setup()
{

  Serial.begin(115200);
delay(2000);

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

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  config.database_url = DATABASE_URL;



  //////////////////////////////////////////////////////////////////////////////////////////////
  //Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  //otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(DATABASE_URL, API_KEY);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
 // Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  timeClient.begin();
  timeClient.setTimeOffset(19800);



}

void printdata()
{
  Serial.print("AirQua1=");
  Serial.print(sensorValue1);               // prints the value read
  Serial.println(" PPM");
  Serial.print("AirQua2=");
  Serial.print(sensorValue2);               // prints the value read
  Serial.println(" PPM");
  Serial.print("TIME: ");
  Serial.println(timeStamp);
}


void loop()
{
    while(!timeClient.update()) {
  timeClient.forceUpdate();
  }

    formattedDate = timeClient.getFormattedDate();
    int splitT = formattedDate.indexOf("T");
  
   timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);

    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      getLoRaData();
      printdata();

      Firebase.setInt(fbdo, "/test/a", sensorValue1.toInt());
      Firebase.setInt(fbdo, "/test/b", sensorValue2.toInt());
       Firebase.setString(fbdo, "/test/Time", timeStamp);
   }
    //Firebase.setInt(fbdo, main, 5);
    
  delay(1000);
}
