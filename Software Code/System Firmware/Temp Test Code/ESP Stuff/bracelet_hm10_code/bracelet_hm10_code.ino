#include <SoftwareSerial.h>
SoftwareSerial mySerial(D5, D6);

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(13, OUTPUT);
  pinMode(2, INPUT);
  mySerial.begin(9600);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {

  char c;
  if (Serial.available()) {
    c = Serial.read();
    mySerial.print(c);
  }
  if (mySerial.available()) {
    c = mySerial.read();
    Serial.print(c);
  }


  if (!digitalRead(2)) {
    digitalWrite(13, HIGH);

  }
  else {
    digitalWrite(13, LOW);
  }
}
