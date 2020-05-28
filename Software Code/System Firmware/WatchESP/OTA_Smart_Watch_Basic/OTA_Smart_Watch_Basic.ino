#include <ESP8266WiFi.h>
#include <WifiEspNow.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//const char* ssid = "..........";
//const char* password = "..........";


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
      motorState = true;
      break;
    case '2':
      digitalWrite(motor, HIGH);
      motorState = true;
      break;
    case '3':
      digitalWrite(motor, HIGH);
      motorState = true;
      break;
    case '4':
      digitalWrite(motor, HIGH);
      motorState = true;
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
  Serial.println("Booting");
 //WIFI INIT
  WiFi.softAP("SMARTSTICKAP", nullptr, 3);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
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
  
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
      ArduinoOTA.handle();

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
      digitalWrite(buzzer, LOW);
      buzzerState = false;
    }
  }
}



