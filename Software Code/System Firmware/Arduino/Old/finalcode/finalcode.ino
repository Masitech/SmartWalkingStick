#include <SoftwareSerial.h>
//creates software serial for esp8266 dev kit
SoftwareSerial mySerial(9, 8); // RX, TX
//MUX Signal Control
const int muxSig3 = 13;
const int muxSig2 = 12;
const int muxSig1 = 7; //

//ultrasonic trigger pin
const int trig = A0;
//Mux read / write pin
const int muxPin = A1;

//Onboard LED pins
const int redLED = 11; //red led
const int greenLED = 6; //green led
const int blueLED = 10; //blue led

//global variable
// there are 4 sensore vaid values are 1 to 4
int whichSensor = 0;
int distance = 0;



void setup() {
  pinMode(muxSig3, OUTPUT);
  pinMode(muxSig2, OUTPUT);
  pinMode(muxSig1, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(muxPin, INPUT);
  mySerial.begin(115200);
  Serial.begin(115200);
}

void loop() {
  ultrasonicProgram();
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
  digitalWrite(redLED, LOW); //RED led off when no object
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

  //listens at the muxPin for return signal 
  //time out at 8000 Microseconds (max distance around 90cm) this is very important otherwise
  //it cause problems with the multiplexers and cannot work with multiple sensor.
  int long duration = pulseIn(muxPin, HIGH, 8000);
  distance = duration * 0.034 / 2;
  delay(1);

}

void takeAction(char whichSensor) {
  // detection range < 30cm >0 is added to remove bug device not connected = 0 or error = 0
  if (distance < 30  && distance > 0) {
    sendESP(whichSensor);
    digitalWrite(redLED, HIGH); //RED led on when theres object
    delay(5);
  }
}

