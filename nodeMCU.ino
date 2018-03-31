#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

String IncomindData = "";
String Temp = "";
boolean isBulkReq = true;

const char* ssid = "redmiSY";
const char* password = "0913937686";

String url = "ec2-18-217-238-61.us-east-2.compute.amazonaws.com:3000/api/testArduino";

SoftwareSerial NodeSerial(D2, D3);  // RX | TX

void setup() {
    // put your setup code here, to run once:
    pinMode(D2, INPUT);
    pinMode(D3, OUTPUT);
    Serial.begin(115200);
    NodeSerial.begin(4800);

    Serial.print("IoT Foundation URL: ");
    Serial.println(url);

    Serial.print("Connecting to: ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.print("nWiFi connected, IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    // put your main code here, to run repeatedly:

//read data from arduino
    while (NodeSerial.available() > 0) {
//      Serial.println("From Nodemcu side Inside while");
        char var = NodeSerial.read();
        Temp = String(var);
        IncomindData += Temp;
    }

    Serial.println(IncomindData);

    HTTPClient http;
    http.begin("http://ec2-18-217-238-61.us-east-2.compute.amazonaws.com:3000/api/testArduino");

    int httpCode = http.GET();                                                                  //Send the request

    if (httpCode > 0) { //Check the returning code
        String
        payload = http.getString();   //Get the request response payload
        //Serial.println(payload);                     //Print the response payload

        if (payload.indexOf("devicePort1") > -1) {
            NodeSerial.print(payload);
            NodeSerial.print("\n");
        }

        if (payload.indexOf("isBulkReq-true") > -1) {
            isBulkReq = true;
        }
    } else {
        Serial.println("Get Bulb Data fails");
    }


    Serial.println("httpCode");
    Serial.println(httpCode);
    http.end();

//Send Bulb kwh Data to the server
    if (isBulkReq) {
        http.begin("http://ec2-18-217-238-61.us-east-2.compute.amazonaws.com:3000/api/testArduino");
        http.addHeader("Content-Type", "application/json");

        StaticJsonBuffer < 300 > JSONbuffer;   //Declaring static JSON buffer
        JsonObject & JSONencoder = JSONbuffer.createObject();

        JSONencoder["deviceMessage"] = IncomindData;

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
        isBulkReq = false;
    }

    IncomindData = "";
    delay(100);
}
