#include <WifiEspNow.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

const int motor = 13; //gpio 13
const int buzzer = 16; //gpio 16

bool motorState = false;
bool buzzerState = false;

unsigned long previousMillis = 0; //last millis value
const long timeOutPeriod = 100;    // 1/10th of a second
unsigned long currentMillis = 0;

void messageDecoder(char c) {
  switch (c) {
    case '1':
      digitalWrite(motor, HIGH);
      tone(buzzer,4000);
      motorState = true;
      buzzerState = true;
      break;
    case '2':
      digitalWrite(motor, HIGH);
      tone(buzzer,6000);
      motorState = true;
      buzzerState = true;
      break;
    case '3':
      digitalWrite(motor, HIGH);
      tone(buzzer,8000);
      motorState = true;
      buzzerState = true;
      break;
    case '4':
      digitalWrite(motor, HIGH);
      tone(buzzer,10000);
      motorState = true;
      buzzerState = true;
      break;
    default:
      break;
  }
}

char receiveVal;
static uint8_t PEER[] {0x2E, 0x3A, 0xE8, 0x36, 0x4C, 0x82};

void printReceivedMessage(const uint8_t mac[6], const uint8_t* buf, size_t count, void* cbarg) {
  //Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (int i = 0; i < count; ++i) {
    receiveVal = static_cast<char>(buf[i]);
  }
  //Serial.println();
  messageDecoder(receiveVal);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

 WiFi.persistent(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("SMARTSTICKAP", nullptr, 3);
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
  //sets pinmode and sets pin to low
  pinMode(motor, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(motor, LOW);
  digitalWrite(buzzer, LOW);
}

void loop() {
  currentMillis = millis();
  updateSensor();
}

void updateSensor() {
  if (currentMillis - previousMillis >= timeOutPeriod) {
    previousMillis = currentMillis;
    if (motorState) {
      digitalWrite(motor, LOW);
      motorState = false;
    }
    if (buzzerState) {
      noTone(buzzer);
      buzzerState = false;
    }
  }
}



