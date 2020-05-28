#include <SoftwareSerial.h>

SoftwareSerial mySerial(9, 8); // RX, TX

const int muxSig3 = 13;
const int muxSig2 = 12;
const int muxSig1 = 7; //
const int trig = A0; //ultrasonic trigger pin
const int muxPin = A1;
const int redLED = 11;


//global variable
int whichSensor = 0; // there are 4 sensore vaid values are 1 to 4
int distance = 0;




void setup() {
  pinMode(muxSig3, OUTPUT);
  pinMode(muxSig2, OUTPUT);
  pinMode(muxSig1, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(muxPin, INPUT);
  mySerial.begin(9600);
  Serial.begin(9600);
}

void loop() {

}

void ultrasonicProgram(){
  readSensor(0, 0, 0, trig); // pin 1 of mux
  takeAction(1);
  readSensor(1, 0, 0, trig); // pin 2 of mux
  takeAction(2);
  readSensor(0, 1, 0, trig); // pin 3 of mux
  takeAction(3);
  readSensor(1, 1, 0, trig); // pin 4 of mux
  takeAction(4);
  digitalWrite(redLED, LOW);
}

void sendESP(int val) {
  mySerial.write(val);
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

void takeAction(int whichSensor) {
  if (distance < 25 && distance > 0) {
    sendESP(whichSensor);
    digitalWrite(redLED, HIGH);
    delay(1);
  }
}

