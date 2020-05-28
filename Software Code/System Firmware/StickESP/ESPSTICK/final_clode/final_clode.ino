#include <WifiEspNow.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(D5, D6); // RX, TX

//----------------------------------------------
// MAC ADDRESS of ESP to Connect & Global Variable
//----------------------------------------------
static uint8_t PEER[] {0x5E, 0xCF, 0x7F, 0x65, 0xA6, 0x89};

char c;

//-----------------------------------------------
// Message Reciving Function
//-----------------------------------------------
void printReceivedMessage(const uint8_t mac[6], const uint8_t* buf, size_t count, void* cbarg) {
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (int i = 0; i < count; ++i) {
    Serial.print(static_cast<char>(buf[i]));
  }
  Serial.println();
 // digitalWrite(LED_PIN, HIGH);
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  Serial.println("Setup");
  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESPNOW", nullptr, 3);
  WiFi.softAPdisconnect(false);
  pinMode(LED_BUILTIN,OUTPUT);
  //Prints this devices mac address
  Serial.print("MAC address of this node is ");
  Serial.println(WiFi.softAPmacAddress());

  //if it cannot establish espnow then the device will restart
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

void sendMess(char mssg[1]) {
  char msg[60];
  int len = snprintf(msg, sizeof(msg), "%s", mssg);
  WifiEspNow.send(PEER, reinterpret_cast<const uint8_t*>(msg), len);
  delay(5);
}

void checkUART() {
  if (mySerial.available() > 0) {
    char ca = mySerial.read();
    Serial.println(ca);
    messageFinder(ca);
     //sendMess(static_cast<char*>(ca));
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
  if (Serial.available() > 0) {
    mySerial.write(Serial.read());
    // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}

//what message
void messageFinder(char c) {
  switch (c) {
    case '1':
    sendMess("1");
      break;
    case '2':
    sendMess("2");   
      break;
    case '3':
    sendMess("3"); 
      break;
    case '4':
    sendMess("4"); 
      break;
    default:
      break;
  }
}


void loop() {
  //digitalWrite(BUILTIN, HIGH); // LED PIN FLASH when data is recived.
  checkUART();
}
