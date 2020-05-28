#include <WifiEspNow.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include <SoftwareSerial.h>
static uint8_t PEER[] {0x5E, 0xCF, 0x7F, 0xB9, 0xCA, 0xBD};

SoftwareSerial mySerial(D5,D6); // RX, TX

void printReceivedMessage(const uint8_t mac[6], const uint8_t* buf, size_t count, void* cbarg) {
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (int i = 0; i < count; ++i) {
    Serial.print(static_cast<char>(buf[i]));
  }
  Serial.println();
}
void setup() {
  
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESPNOW", nullptr, 3);
  WiFi.softAPdisconnect(false);

  Serial.print("MAC address of this node is ");
  Serial.println(WiFi.softAPmacAddress());

  bool ok = WifiEspNow.begin();
  if (!ok) {
    Serial.println("WifiEspNow.begin() failed");
    ESP.restart();
  }

  WifiEspNow.onReceive(printReceivedMessage, nullptr);

  ok = WifiEspNow.addPeer(PEER);
  if (!ok) {
    Serial.println("WifiEspNow.addPeer() failed");
    ESP.restart();
  }

}

void loop() {
  if (mySerial.available()>0) {
    Serial.write(mySerial.read());
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    sendMess();
  }
  if (Serial.available()>0) {
    mySerial.write(Serial.read());
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
  
}


 void sendMess(){
  char msg[60];
  int len = snprintf(msg, sizeof(msg), "hello ESP-NOW from %s at %lu", WiFi.softAPmacAddress().c_str(), millis());
  WifiEspNow.send(PEER, reinterpret_cast<const uint8_t*>(msg), len);
  delay(10);
 }




