#include <SoftwareSerial.h>
#include "RunningAverage.h" //
SoftwareSerial mySerial(9, 8); // RX, TX

const int muxSig3 = 13;
const int muxSig2 = 12;
const int muxSig1 = 7; //
const int trig = A0; //ultrasonic trigger pin
const int muxPin = A1;
const int redLED = 11;

//global variable
long int duration;
int s1Sample = 0;
int s2Sample = 0;
int s3Sample = 0;
int s4Sample = 0;

//running average
RunningAverage sensor1(10);
RunningAverage sensor2(10);
RunningAverage sensor3(10);
RunningAverage sensor4(10);
int samples = 0;


void setup() {
  pinMode(muxSig3, OUTPUT);
  pinMode(muxSig2, OUTPUT);
  pinMode(muxSig1, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(muxPin, INPUT);
  mySerial.begin(9600);

  sensor1.clear(); // explicitly start clean
  sensor2.clear(); // explicitly start clean
  sensor3.clear(); // explicitly start clean
  sensor4.clear(); // explicitly start clean
  
  Serial.begin(9600);
}

void loop() {
  readSensor(0, 0, 0, trig, sensor1, s1Sample);
  readSensor(0, 0, 1, trig, sensor2, s2Sample);
  readSensor(0, 1, 0, trig, sensor3, s3Sample);
  readSensor(0, 1, 1, trig, sensor4, s4Sample);

  takeAction();

  digitalWrite(redLED, LOW);
}


void sendESP(int val){
  mySerial.write(val);
  digitalWrite(redLED,HIGH);
}

boolean readSensor(int s1, int s2, int s3, int trig, RunningAverage myRA,int sampleSize) {
  // set deMux channel
  digitalWrite(muxSig1, s1);
  digitalWrite(muxSig2, s2);
  digitalWrite(muxSig3, s3);

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(muxPin, HIGH);
  myRA.addValue((duration * 0.034 / 2));
  sampleSize++;
  if(sampleSize > 250){
    myRA.clear();
  }
}

void takeAction(){
  if(sensor1.getAverage(),3 < 20){
    sendESP(1);
     digitalWrite(redLED, HIGH);
  }
}

