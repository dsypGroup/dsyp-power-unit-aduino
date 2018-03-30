#include <SoftwareSerial.h>

SoftwareSerial ArduinoSerial(3, 2);  // Rx, TX

int ledPin = 8;
String IncomindData = "";
String Temp = "";

  const int analogIn = A0;
  int mVperAmp = 100; // use 100 for 20A Module and 66 for 30A Module
  int RawValue= 0;
  int ACSoffset = 2500;
  double Voltage = 0;
  double Amps = 0;
  float power = 0;
  float energy_KWH = 0;
  uint32_t lastTime=0;
  uint32_t currentTime=0;
  uint32_t timeInterval=1000;
  String motorStatus = "Off";
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ArduinoSerial.begin(4800);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  currentTime=millis();
  // put your main code here, to run repeatedly:
  
    while (ArduinoSerial.available() > 0) {
      Serial.println("From arduino side Inside while");
      char var = ArduinoSerial.read();
      Temp = String(var);
      IncomindData+= Temp;

//      if (ArduinoSerial.read() =='\n') {
//        Serial.println("From arduino side");
//        Serial.println(val);
//
//        if(val == 10) {
//          Serial.print("Turn Off The Bulb1");
//          motorStatus = "Off";
////          digitalWrite(ledPin, LOW);
//        } else if (val == 11) {
//          Serial.print("Turn On The Bulb1");
//          motorStatus = "On";
////          digitalWrite(ledPin, HIGH);
//        } else if (val == 404) {
//          Serial.print("Turn Off The Bulb2");
//          motorStatus = "Off";
//          digitalWrite(ledPin, LOW);
//        } else if (val == 200) {
//          Serial.print("Turn On The Bulb2");
//          motorStatus = "On";
//          digitalWrite(ledPin, HIGH);
//        } else {
//          Serial.print("TUnmatched value returned");
//          Serial.print(val);
//        }
//      }

    }
    
    Serial.println(IncomindData);
    IncomingData = "";
   

//Power Consumption Code

if(motorStatus == "On") {
  RawValue = analogRead(analogIn);
  Voltage = (RawValue / 1024.0) * 5000; // Gets you mV
  Amps = ((Voltage - ACSoffset) / mVperAmp);

  power = (Voltage/1000)*Amps;
  energy_KWH = energy_KWH + (power/1000)* (currentTime - lastTime)/(60*60*1000);

Serial.print("Raw Value = " ); // shows pre-scaled value
Serial.print(RawValue);
Serial.print("\t mV = "); // shows the voltage measured
Serial.print(Voltage,3);
// the '3' after voltage allows you to display 3 digits after decimal point
Serial.print("\t Amps = "); // shows the voltage measured
Serial.print(Amps,3);
// the '3' after voltage allows you to display 3 digits after decimal point

Serial.print("  Power Value = " );
Serial.print(power); 

Serial.print("  KWH  Value = " );
Serial.print(energy_KWH,6); 
Serial.println();

  lastTime =millis(); 
}

    

// swnd data to nodemcu
    ArduinoSerial.print(energy_KWH, 6);
    ArduinoSerial.print("\n");
    
    delay(100);
}
