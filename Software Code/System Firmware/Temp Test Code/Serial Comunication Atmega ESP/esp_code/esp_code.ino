#include <SoftwareSerial.h>
//Serial for arduino connection
SoftwareSerial mySerial(D5,D6); // RX, TX


void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  //mySerial.write('1');
  delay(100);
  if (mySerial.available()>0) {
    Serial.write(mySerial.read());
    //When a new message is recived change the state of the built in led. 
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    //sendMess();
  }
  
  if (Serial.available()>0) {
    mySerial.write(Serial.read());
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}





