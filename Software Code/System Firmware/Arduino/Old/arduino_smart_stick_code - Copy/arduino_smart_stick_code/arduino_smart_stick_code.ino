//#include "Ticker.h"
#include <SoftwareSerial.h>
#include <string.h>
#include <ArduinoHttpClient.h>
#define TINY_GSM_MODEM_SIM800

//------------------------------------------------------------
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif
MPU6050 mpu;
#define TINY_GSM_MODEM_SIM800
// Increase RX buffer if needed
// quaternion components in a [w, x, y, z] format (not best for parsing
// on a remote host such as Processing or something though)
#define OUTPUT_READABLE_QUATERNION
#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
bool blinkState = false;
// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
unsigned long currentMillis = 0;
unsigned int ival = 3000;
unsigned int timeOut = 6000; //this means roughly 6 second window :)
bool startTimer = false;

//-------------------------------------------------------------

SoftwareSerial mySerial(9, 8); // RX, TX

//Multiplexer setting
const int muxSig3 = 13;
const int muxSig2 = 12;
const int muxSig1 = 7; //
const int trig = A0; //ultrasonic trigger pin
const int muxPin = A1;

//LED lights
const int redLED = 11;
const int greenLED = 10;
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
#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, server, port);


void setup() {
  mySerial.begin(115200);
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);
  // Set GSM module baud rate
  TinyGsmAutoBaud(SerialAT);

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

  //digitalWrite(simVReg, HIGH); //enables the sim module by deafult.
  //sim800lSetUp();
  delay(5000);
  connectGPRS(); // start the internet
  delay(5000);
  //  sendSQL.start();
  //  closeSQL.start();
  MPUsetup();
}

// MAIN LOOP
unsigned long previousupdate = 0;

//bool noInternet = true;
unsigned long currentT = 0;
int intervalCheck = 500;
void loop() {
    simModuleChecks();
  if ((millis() - currentT) > intervalCheck) {
    currentT = millis();
    while ((millis() - currentT) < 100) {
      MPUloop();
    }
    checkFlat();
  }
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
  delay(1000);
  updateSQL();
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


volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

void timerSetUp() {
  if (!startTimer) {
    currentMillis = millis();
    startTimer = true;
  }
}

void MPUsetup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  // Wire.setClock(1000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // initialize device
  SerialMon.println(F("Initializing I2C devices..."));
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  // verify connection
  SerialMon.println(F("Testing device connections..."));
  SerialMon.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  // load and configure the DMP
  SerialMon.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    SerialMon.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    SerialMon.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    SerialMon.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    SerialMon.print(F("DMP Initialization failed (code "));
    SerialMon.print(devStatus);
    SerialMon.println(F(")"));
  }
}

void MPUloop() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;


  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;

    // display quaternion values in easy matrix form: w x y z
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    Serial.print("quat\t");
    Serial.print(q.w);
    Serial.print("\t");
    Serial.print(q.x);
    Serial.print("\t");
    Serial.print(q.y);
    Serial.print("\t");
    Serial.println(q.z);
  }
}
void checkFlat() {
  if (q.x < 0.1 && q.x > -0.1 && q.y < 0.1 && q.y > -0.1) {
    //startTimer = true; //by deafult its false;
    //Serial.println("XD");

    if (startTimer == false) {
      timerSetUp();
    }
    if (startTimer && 0.1 && q.x > -0.1 && q.y < 0.1 && q.y > -0.1 && (millis() - currentMillis) > ival && (millis() - currentMillis) < timeOut) {
      Serial.println("AM FLAT For 4 Seconds");
      startTimer = false;
      digitalWrite(blueLED, !digitalRead(blueLED));
      if ((millis() - previousupdate) > 20000) {
        previousupdate =  millis();
        sendSMSUpdateSQL();
      }
    }
    if ((millis() - currentMillis) > timeOut) {
      startTimer = false;
    }
  }
}


