#include <SoftwareSerial.h>

SoftwareSerial mySerial(9, 8); // RX, TX
int LED_BUILTIN1 = 6;
void setup() {
  
  Serial.begin(115200);
  mySerial.begin(115200);
  pinMode(LED_BUILTIN1, OUTPUT);


}

void loop() {
  if (mySerial.available()>0) {
    Serial.write(mySerial.read());
    digitalWrite(LED_BUILTIN1, !digitalRead(LED_BUILTIN1));
  }
  if (Serial.available()>0) {
    mySerial.write(Serial.read());
    digitalWrite(LED_BUILTIN1, !digitalRead(LED_BUILTIN1));
  }
  
}
 



