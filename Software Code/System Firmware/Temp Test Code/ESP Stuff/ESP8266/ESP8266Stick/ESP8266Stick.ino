#include <WifiEspNowBroadcast.h>
#include <WifiEspNow.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

static const int trigPin = 5;//D1
static const int echoPin = 4;//D3
long duration;
int distance;

static const int BUTTON_PIN = 0; // "flash" button on NodeMCU, Witty Cloud, etc
static const int LED_PIN = 2;    // ESP-12F blue LED

int ledState = HIGH;
//-------------------------
// Message Reciving Function
//-------------------------
void processRx(const uint8_t mac[6], const uint8_t* buf, size_t count, void* cbarg) {
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (int i = 0; i < count; ++i) {
    Serial.print(static_cast<char>(buf[i]));
  }
  Serial.println();

  digitalWrite(LED_PIN, ledState);
  ledState = !ledState;  // was high now low was low now high
}

//-------------------------
// Message Sending Function
//-------------------------
void sendMessage() {
  char msg[60];
  int len = snprintf(msg, sizeof(msg), "Message from Walking Stick: Object Detected");
  WifiEspNowBroadcast.send(reinterpret_cast<const uint8_t*>(msg), len);

  Serial.println("Sending message");
  Serial.println(msg);
  Serial.print("Recipients:");
  const int MAX_PEERS = 5;
  WifiEspNowPeerInfo peers[MAX_PEERS];
  int nPeers = std::min(WifiEspNow.listPeers(peers, MAX_PEERS), MAX_PEERS);
  for (int i = 0; i < nPeers; ++i) {
    //Serial.printf(" %02X:%02X:%02X:%02X:%02X:%02X", peers[i].mac[0], peers[i].mac[1], peers[i].mac[2], peers[i].mac[3], peers[i].mac[4], peers[i].mac[5]);
  }
  Serial.println();
}
//-------------------------
// Setup
//-------------------------
void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  WiFi.persistent(false);
  bool ok = WifiEspNowBroadcast.begin("ESPNOW", 3);
  if (!ok) {
    Serial.println("WifiEspNowBroadcast.begin() failed");
    ESP.restart();
  }

  WifiEspNowBroadcast.onReceive(processRx, nullptr);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);

  Serial.print("MAC address of this node is ");
  Serial.println(WiFi.softAPmacAddress());
  Serial.println("Press the button to send a message");
}

//-------------------------
//Main Loop
//-------------------------
void loop() {
ultrasonicSensor();
if(distance < 40){
  sendMessage();
}
else{
  WifiEspNowBroadcast.loop();
  delay(10);
}
}
//Code for ultrasonic Sensor on ESP for testing purpose only
void ultrasonicSensor(){

  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  
  Serial.print(distance);
 // Serial.println("Centimeter:");

}

