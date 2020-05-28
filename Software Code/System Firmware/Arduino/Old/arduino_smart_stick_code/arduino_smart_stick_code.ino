//#include "Ticker.h"
#include <SoftwareSerial.h>
#include <string.h>
#include <ArduinoHttpClient.h>
#define TINY_GSM_MODEM_SIM800

//Multiplexer setting
const int muxSig3 = 13;//13
const int muxSig2 = 12; //
const int muxSig1 = 7; //
const int trig = A0; //ultrasonic trigger pin
const int muxPin = A1;

//LED lights
const int redLED = 11;
const int greenLED = 10; //10
const int blueLED = 6;

//global variable
int whichSensor = 0; // there are 4 sensore vaid values are 1 to 4
int distance = 0;
const int arduinoButton = 1; //on physical pin 3

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Geo Location Variable
String message;
String latitude = "0";
String longitude = "0";
String currentTime = "0";
String statusx = "1";
String Date = "0";
String Time = "0";
String distancesql = "33";
String heartRate = "80";
//----------------------tinyGSM-------------------------
// Select your modem:
#define TINY_GSM_MODEM_SIM800

// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialMon Serial

SoftwareSerial SerialAT(5, 4); // RX, TX
SoftwareSerial mySerial(9, 8); // RX, TX
//#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG SerialMon

//sim moudule setting
const int simVReg = 0;
String contactNumber1 = "07410124124";
String contactNumber2 = "";

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "giffgaff.com";
const char user[] = "giffgaff";
const char pass[] = "";

// Server details
String server = "smart-stick.co.uk";
String resource = "";
const int  port = 80;
unsigned long lastPostCount = 0; //closes connect after 10 seconds
unsigned long postInterval = 10000; //10 seconds
bool sqlUpdated = false;

//Ticker sendSQL(sendSMSUpdateSQL, 20000);             //---------------------ticker
//Ticker closeSQL(simModuleChecks, 10000);

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, server, port);


void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);
  // Set GSM module baud rate
  TinyGsmAutoBaud(SerialAT);
  mySerial.begin(115200);
  
  pinMode(muxSig3, OUTPUT);
  pinMode(muxSig2, OUTPUT);
  pinMode(muxSig1, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(muxPin, INPUT);
  pinMode(simVReg, OUTPUT);
  //pinMode(arduinoButton, INPUT);

  digitalWrite(simVReg, HIGH); //enables the sim module by deafult.
  delay(5000);
  connectGPRS(); // start the internet
  delay(5000);
  //  sendSQL.start();
  //  closeSQL.start();

}

// MAIN LOOP
unsigned long previousupdate = 0;

void loop() {
    delay(5000);
  connectGPRS(); // start the internet
  delay(20000);
 // ultrasonicProgram();
  //  sendSQL.update(); //ticker
  //  closeSQL.update();
  //simModuleChecks();
  
  //checkUART();
  digitalWrite(redLED, HIGH); 
  //SerialAT.listen();
}

//code takes a string and separates it based on a given
//character and returns The item between the separating character
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
// sim functionality here
void simModuleChecks() {
  if ((millis() - lastPostCount) > postInterval && sqlUpdated == true) {
    http.stop();
    digitalWrite(redLED, LOW);
    sqlUpdated = false;
  }

}
//this function checks if theres http connection already if so then closes , then get gps location, send sms, and update sql
void sendSMSUpdateSQL() {
  simModuleChecks();
  message = getGPSLocation();
  longitude = getValue(message, ',', 1);
  latitude = getValue(message, ',', 2);
  Date = getValue(message, ',', 3);
  Time = getValue(message, ',', 4);
  sendSMS("Longitude: " + longitude + " Latitude: " + latitude + " Date/Time: " + Date + Time + " Heart Rate: " + heartRate); //gets gps location
  delay(500);
  updateSQL();
  delay(500);
}


//make a call
bool makeCall(String number) {
  if (modem.isNetworkConnected()) {
    return modem.callNumber(number);
  }
  else {
    return false;
  }
}

//send sms
bool sendSMS(String message) {
  //String imei = modem.getIMEI();
  if (modem.isNetworkConnected()) {
    return modem.sendSMS(contactNumber1, message);
  }
  else {
    return false;
  }
}

bool connectGPRS() {
  if (isNetwork()) { //if connected to network connect to internet.
    if (!modem.isGprsConnected()) { // if gprs is not connected then connect else return
      if (modem.gprsConnect(apn, user, pass)) {
        delay(3000);
        return true;
      }
      return true;
    }
    else if (modem.isGprsConnected()) { //if already connected to internet
      return true;
    }
  }
  else {
    return false;
  }
}

String getGPSLocation() {
  if (connectGPRS()) {
    return modem.getGsmLocation();
  }
  else {
    return"error";
  }
}

void updateSQL() {
  resource = "/update.php?TIME=" + Time + "&DATE=" + Date + "&DISTANCE=" + distancesql + "&HEARTRATE=" + heartRate + "&LATITUDE=" + latitude + "&LONGITUDE=" + longitude + "&STATUS=" + statusx;
  http.get(resource);
  delay(500);
  lastPostCount = millis(); // saves the timer at that moment
  sqlUpdated = true;
  digitalWrite(redLED, HIGH);

}

bool isNetwork() {
  return modem.isNetworkConnected();
}

int signalQulity() {
  return modem.getSignalQuality();

}

void ultrasonicProgram() {
  readSensor(0, 0, 0, trig); // pin 1 of mux
  takeAction('1');
  readSensor(1, 0, 0, trig); // pin 2 of mux
  takeAction('2');
  readSensor(0, 1, 0, trig); // pin 3 of mux
  takeAction('3');
  readSensor(1, 1, 0, trig); // pin 4 of mux
  takeAction('4');
  digitalWrite(redLED, LOW);
  //digitalWrite(greenLED, HIGH);
  //digitalWrite(blueLED, HIGH);
}

void sendESP(char val) {
  mySerial.write(val);
  mySerial.flush();
}

boolean readSensor(int s1, int s2, int s3, int trig) {
  // set deMux channel
  digitalWrite(muxSig1, s1);
  digitalWrite(muxSig2, s2);
  digitalWrite(muxSig3, s3);

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  int long duration = pulseIn(muxPin, HIGH, 8000);
  distance = duration * 0.034 / 2;
  delay(1);

}

void takeAction(char whichSensor) {
  if (distance < 30  && distance > 0) {
    sendESP(whichSensor);
    digitalWrite(redLED, HIGH);
    delay(5);
  }
}

void checkUART() {
  mySerial.listen();
  if (mySerial.available() > 0) {
    char ca = mySerial.read();
    //Serial.println(ca);
    messageFinder(ca);
    digitalWrite(blueLED, !digitalRead(blueLED));
  }
}

//what message
void messageFinder(char c) {
  switch (c) {
    case '1':
      if ((millis() - previousupdate) > 6000) {
        sendSMSUpdateSQL();
        previousupdate =  millis();
        digitalWrite(greenLED, !digitalRead(greenLED));
      }
      break;
    case '2':

      break;
    case '3':

      break;
    case '4':

      break;
    default:
      break;
  }
}

