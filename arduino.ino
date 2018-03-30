#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

const char* ssid = "redmiSY";
const char* password = "0913937686";

String url = "ec2-18-217-238-61.us-east-2.compute.amazonaws.com:3000/api/testArduino";
String deviceStatus = "Off";
int ledPin = 2;

uint32_t lastTime=0;
uint32_t currentTime=0;
float bulb2Units = 0;

SoftwareSerial NodeSerial(D2, D3);  // RX | TX
void setup() {
  // put your setup code here, to run once:
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  Serial.begin(115200);
  NodeSerial.begin(4800);

   Serial.print("IoT Foundation URL: "); 
   Serial.println(url);
   
//  pinMode(ledPin, OUTPUT);
//  digitalWrite(ledPin, LOW);

   Serial.print("Connecting to: "); 
   Serial.print(ssid);
   WiFi.begin(ssid, password); 
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }

   Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void loop() {
  currentTime=millis();
  // put your main code here, to run repeatedly:

  //Send data to arduino
//  NodeSerial.print(10);
//  NodeSerial.print("\n");

//read data from arduino
  while (NodeSerial.available() > 0) {
      bulb2Units= NodeSerial.parseFloat();
      if (NodeSerial.read() =='\n') {
        Serial.print("From Node side");
        Serial.println(bulb2Units,6);
      }
    }

   HTTPClient http;
   http.begin("http://ec2-18-217-238-61.us-east-2.compute.amazonaws.com:3000/api/testArduino");

   int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode > 0) { //Check the returning code  
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
//      NodeSerial.print(payload);
//      NodeSerial.print("\n"); 

//      int len=payload.length();
//      Serial.println(len); 
//       
//      char  buf[len + 1];
//      payload.toCharArray(buf, len + 1);        
//      int bufLength = sizeof(buf);
//      Serial.println(bufLength); 

      if(payload.indexOf("bulb2-On") > -1) {
//        digitalWrite(ledPin, HIGH);
        Serial.println("device on"); 
        NodeSerial.print(200);
        NodeSerial.print("\n");
      } 

      if(payload.indexOf("bulb2-Off") > -1) {
//        digitalWrite(ledPin, LOW);
        Serial.println("device off");
        NodeSerial.print(404);
        NodeSerial.print("\n"); 
      } 
    } else {
      Serial.println("Get Bulb Data fails");
    }
    

   Serial.println("httpCode"); 
   Serial.println(httpCode); 
   http.end();
   
//Send Bulb kwh Data to the server
//  if (currentTime > 20000 && currentTime < 40000) {
//    HTTPClient http;
    http.begin("http://ec2-18-217-238-61.us-east-2.compute.amazonaws.com:3000/api/testArduino");
//    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Content-Type", "application/json");
    
    StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
    JsonObject& JSONencoder = JSONbuffer.createObject(); 
 
    JSONencoder["deviceName"] = "bulb2";
    JSONencoder["deviceUnits"] = bulb2Units;

//    JsonArray& values = JSONencoder.createNestedArray("values"); //JSON array
//    values.add(20); //Add value to array
//    values.add(21); //Add value to array
//    values.add(23); //Add value to array
// 
//    JsonArray& timestamps = JSONencoder.createNestedArray("timestamps"); //JSON array
//    timestamps.add("10:10"); //Add value to array
//    timestamps.add("10:20"); //Add value to array
//    timestamps.add("10:30"); //Add value to array

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
   
   int httpCode2 = http.POST(JSONmessageBuffer);
   String payload2 = http.getString(); 

    Serial.print("POST payload: "); 
    Serial.println(payload2);
    Serial.print("HTTP POST Response: "); 
    Serial.println(httpCode2); // HTTP code 200 means ok
    http.end();
//  }

        NodeSerial.print("ABCDEFGHIJKLMNOPQRSTUVWXY");
        NodeSerial.print("\n");

  delay(100);
}
